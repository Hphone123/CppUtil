#pragma once

#include <exception>
#include <future>
#include <memory>
#include <thread>
#include <condition_variable>
#include <type_traits>

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

  // Declare and define an async function.
  // Will also create a function nanmed __async__func_name
  // Only works outside classes.
  #define __async__(ret_type, func_name, ...)                                                                                                   \
    ret_type __async__##func_name(__VA_ARGS__);                                                                                                 \
                                                                                                                                                \
    template<typename... Args>                                                                                                                  \
    CppUtil::Promise<ret_type> func_name (Args&&... a)                                                                                          \
    {                                                                                                                                           \
      static_assert(std::is_invocable_r_v<ret_type, decltype(__async__##func_name), a>,                                                         \
        "Function " #func_name " has signature " #ret_type "(" #__VA_ARGS__ ")");                                                               \
      return CppUtil::Async::async<ret_type>(__async__##func_name, a);                                                                          \
    }                                                                                                                                           \
                                                                                                                                                \
    ret_type __async__##func_name(__VA_ARGS__)

  // Declare an async member function.
  // Will also declare a private a private member function named `__async__func_name`
  // implementation must be outside of class body, or in another file.
  // Inline implementations must be done by hand.
  // Only works inside a class body.
  #define __async_member_decl__(ret_type, func_name, ...)                                                                                       \
    private:                                                                                                                                    \
      ret_type __async__##func_name(__VA_ARGS__);                                                                                               \
                                                                                                                                                \
    public:                                                                                                                                     \
                                                                                                                                                \
      template<typename... Args>                                                                                                                \
      CppUtil::Promise<ret_type> func_name(Args&&... a)                                                                                         \
      {                                                                                                                                         \
        using Self = std::remove_reference_t<decltype(*this)>;                                                                                  \
        static_assert(std::is_invocable_v<decltype(&Self::__async__##func_name), Self&, Args...>,                                               \
          "Function: "  #ret_type " " #func_name "(" #__VA_ARGS__ ") is not invocable with given arguments; see compiler log for more info!");  \
        return CppUtil::Async::async<ret_type>([this](Args&&... a) -> ret_type{return this->__async__##func_name(a...);}, a...);                \
      }                                                                                                                         

  // Implementation of an async function
  // Must be placed outside the classes body.
  #define __async_member_impl__(ret_type, class_name, func_name, ...)                                                                           \
    ret_type class_name :: __async__##func_name(__VA_ARGS__)

  // Call the async function and await its return
  #define __await__(func_name, ...)                                                                                                             \
    func_name(__VA_ARGS__).get()
}


