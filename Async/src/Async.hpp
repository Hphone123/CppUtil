#pragma once

#include <exception>
#include <memory>
#include <thread>
#include <condition_variable>

namespace CppUtil
{
  struct AsyncState
  {
    std::exception_ptr      exc;

    bool                    threw;
    bool                    finished;

    std::condition_variable cv;
    std::mutex              mtx;
  };

  template<typename T>
  class Promise 
  {
  private:
    
    std::shared_ptr<AsyncState> state;

    std::shared_ptr<T>          value;

  public:

    Promise<T>(std::shared_ptr<AsyncState> state, std::shared_ptr<T> value):
      state(state),
      value(value)
    {}

    bool isFinished()
    {
      return state->finished;
    }

    T get()
    {
      std::unique_lock<std::mutex> lock(state->mtx);
      state->cv.wait(lock, [this]{return (state->finished);});
      if (state->threw)
        std::rethrow_exception(state->exc);
      return *value;
    }

    operator T ()
    {
      return this->get();
    }
  };

  template<>
  class Promise<void> 
  {
  private:
    
    std::shared_ptr<AsyncState>  state;

  public:

    Promise<void>(std::shared_ptr<AsyncState> state):
      state(state)
    {}

    bool isFinished()
    {
      return state->finished;
    }

    void get()
    {
      std::unique_lock<std::mutex> lock(state->mtx);
      state->cv.wait(lock, [this]{return (state->finished);});
      if (state->threw)
        std::rethrow_exception(state->exc);
      return;
    }
  };

  template<typename T>
  class Future
  {
  private:
    
    std::shared_ptr<AsyncState> state;

    std::shared_ptr<T>          value; 

  public:

    Future<T>():
      state(std::make_shared<AsyncState>()),
      value(std::make_shared<T>())
    {}
    
    CppUtil::Promise<T> asPromise()
    {
      return Promise<T>(state, value);
    }

    void setValue(T v)
    {
      *value = v;
    }

    void finish()
    {
      state->finished = true;
      state->cv.notify_all();
    }

    void setThrow(std::exception_ptr v)
    {
      state->exc = v;
      state->threw = true;
    }
  };

  template<>
  class Future<void>
  {
  private:
    
    std::shared_ptr<AsyncState> state;

  public:

    Future<void>():
      state(std::make_shared<AsyncState>())
    {}
    
    CppUtil::Promise<void> asPromise()
    {
      return Promise<void>(state);
    }

    void finish()
    {
      state->finished = true;
      state->cv.notify_all();
    }

    void setThrow(std::exception_ptr v)
    {
      state->exc = v;
      state->threw = true;
    }
  };

  class Async 
  {
  private:
    template<typename T, typename F, typename... A>
    static void _async(Future<T> res, F f, A... a) 
    {
      try 
      {
        using ResultT = decltype(std::apply(f, a...));
        if constexpr (std::is_void_v<ResultT>) 
        {
          std::apply(f, a...);
        } 
        else 
        {
          auto r = std::apply(f, a...);
          res.setValue(std::move(r));
        }
      } 
      catch (...) 
      {
        res.setThrow(std::current_exception());
      }
      res.finish();
    }

  public:
    template<typename T, typename Func, typename... Args>
    static Promise<T> async(Func&& f, Args&&... a) 
    {
      static_assert(std::is_invocable_r_v<T, Func, Args...>,
                    "Async function must return T and accept the given arguments!");

      auto res  = Future<T>();

      using Fn  = std::decay_t<Func>;
      using Tup = std::tuple<std::decay_t<Args>...>;

      auto fn   = Fn (std::forward<Func>(f));
      auto args = Tup(std::forward<Args>(a)...);
      
      std::thread(
            [res, fn = std::move(fn), args = std::move(args)]() mutable {
                _async<T>(res, std::move(fn), std::move(args));
            }
        ).detach();
      return res.asPromise();
    }
  };

  #define __async__(ret_type, func_name, ...)                                                                                   \
    template<typename... Args>                                                                                                  \
    CppUtil::Promise<ret_type> func_name(Args&&... a);                                                                          \
                                                                                                                                \
    ret_type _##func_name(__VA_ARGS__);                                                                                         \
                                                                                                                                \
    template<typename... Args>                                                                                                  \
    CppUtil::Promise<ret_type> func_name(Args&&... a)                                                                           \
    {                                                                                                                           \
      static_assert(std::is_invocable_r_v<ret_type, decltype(_##func_name), Args...>,                                           \
        "'"#func_name "' has signature: '"#ret_type "(" #__VA_ARGS__ ")'!");                                                    \
      return CppUtil::Async::async<ret_type>(_##func_name, a...);                                                               \
    }                                                                                                                           \
                                                                                                                                \
    ret_type _##func_name(__VA_ARGS__)

  #define __await__(func_name, ...)                                                                                             \
    func_name(__VA_ARGS__).get()
    
}


