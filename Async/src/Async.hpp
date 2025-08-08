#pragma once

#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <thread>

namespace CppUtil
{
  template<typename T>
  class Promise 
  {
  private:
    T                       value;
    std::exception_ptr      exc;

    bool                    threw;
    bool                    finished;

    std::condition_variable cv;
    std::mutex              mtx;

  public:
    void setValue(T v)
    {
      value = v;
    };

    void setExc(std::exception_ptr eptr)
    {
      threw = true;
      exc = eptr;
    };

    void finish()
    {
      this->finished = true;
      cv.notify_all();
    }

    bool isFinished()
    {
      return finished;
    }

    T get()
    {
      std::unique_lock lock(mtx);
      cv.wait(lock, [this]{return (this->finished);});
      if (this->threw)
        std::rethrow_exception(exc);
      return this->value;
    }
  };

  class Async 
  {
  private:
    template<typename T, typename F, typename... A>
    static void _async(std::shared_ptr<Promise<T>> res, F f, A... a) 
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
          auto r = std::apply(f, a...);   // rvalue
          res->setValue(std::move(r)); // move into promise
        }
      } 
      catch (...) 
      {
        res->setExc(std::current_exception());
      }
      res->finish();
    }

  public:
    template<typename T, typename Func, typename... Args>
    static std::shared_ptr<Promise<T>> async(Func&& f, Args&&... a) 
    {
      static_assert(std::is_invocable_r_v<T, Func, Args...>,
                    "Async function must return T and accept the given arguments!");

      auto res  = std::make_shared<Promise<T>>();

      using Fn  = std::decay_t<Func>;
      using Tup = std::tuple<std::decay_t<Args>...>;

      auto fn   = Fn (std::forward<Func>(f));
      auto args = Tup(std::forward<Args>(a)...);

      // Prefer lambda to avoid function-pointer template headaches:
      std::thread(
            [res, fn = std::move(fn), args = std::move(args)]() mutable {
                _async<T>(res, std::move(fn), std::move(args));
            }
        ).detach();
      return res;
    }
  };
    
}

