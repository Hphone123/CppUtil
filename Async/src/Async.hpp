#pragma once

#include <condition_variable>
#include <exception>
#include <memory>
#include <thread>
#include <type_traits>

namespace CppUtil
{
struct AsyncState
{
  std::exception_ptr exc;

  bool threw;
  bool finished;

  std::condition_variable cv;
  std::mutex              mtx;
};

template <typename T> class Promise
{
private:
  std::shared_ptr<AsyncState> state;

  std::shared_ptr<T> value;

public:
  Promise<T>(std::shared_ptr<AsyncState> state, std::shared_ptr<T> value) : state(state), value(value) {}

  bool isFinished()
  {
    return state->finished;
  }

  T get()
  {
    std::unique_lock<std::mutex> lock(state->mtx);
    state->cv.wait(lock, [this] { return (state->finished); });
    if (state->threw)
      std::rethrow_exception(state->exc);
    return *value;
  }

  operator T()
  {
    return this->get();
  }
};

template <> class Promise<void>
{
private:
  std::shared_ptr<AsyncState> state;

public:
  Promise<void>(std::shared_ptr<AsyncState> state) : state(state) {}

  bool isFinished()
  {
    return state->finished;
  }

  void get()
  {
    std::unique_lock<std::mutex> lock(state->mtx);
    state->cv.wait(lock, [this] { return (state->finished); });
    if (state->threw)
      std::rethrow_exception(state->exc);
    return;
  }
};

template <typename T> class Future
{
private:
  std::shared_ptr<AsyncState> state;

  std::shared_ptr<T> value;

public:
  Future<T>() : state(std::make_shared<AsyncState>()), value(std::make_shared<T>()) {}

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
    state->exc   = v;
    state->threw = true;
  }
};

template <> class Future<void>
{
private:
  std::shared_ptr<AsyncState> state;

public:
  Future<void>() : state(std::make_shared<AsyncState>()) {}

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
    state->exc   = v;
    state->threw = true;
  }
};

class Async
{
private:
  template <typename T, typename F, typename... A> static void _async(Future<T> res, F f, A... a)
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
  template <typename T, typename Func, typename... Args> static Promise<T> async(Func&& f, Args&&... a)
  {
    static_assert(std::is_invocable_r_v<T, Func, Args...>,
                  "Async function must return T and accept the given arguments!");

    auto res = Future<T>();

    using Fn  = std::decay_t<Func>;
    using Tup = std::tuple<std::decay_t<Args>...>;

    auto fn   = Fn(std::forward<Func>(f));
    auto args = Tup(std::forward<Args>(a)...);

    std::thread([res, fn = std::move(fn), args = std::move(args)]() mutable
                { _async<T>(res, std::move(fn), std::move(args)); })
      .detach();
    return res.asPromise();
  }
};

// Declare and define an async function.
// Will also create a function nanmed __async__func_name
// Only works outside classes.
#define __async__(ret_type, func_name, ...)                                                                            \
  ret_type __async__##func_name(__VA_ARGS__);                                                                          \
                                                                                                                       \
  template <typename... Args> CppUtil::Promise<ret_type> func_name(Args&&... a)                                        \
  {                                                                                                                    \
    static_assert(std::is_invocable_r_v<ret_type, decltype(__async__##func_name), Args...>,                            \
                  "Function " #func_name " has signature " #ret_type "(" #__VA_ARGS__ ")");                            \
    return CppUtil::Async::async<ret_type>(__async__##func_name, a...);                                                \
  }                                                                                                                    \
                                                                                                                       \
  ret_type __async__##func_name(__VA_ARGS__)

// Declaration of an async function
// Will also declare a private member function named `__async__func_name`
// Use `__async_member_impl__` to add the implementation for `__async__func_name` and `func_name`.
// The scope after this macro is public.
// Only works inside a class body.
#define __async_member_decl__(ret_type, func_name, ...)                                                                \
private:                                                                                                               \
  /* 1) The actual implementation (can have default args!) */                                                          \
  ret_type __async__##func_name(__VA_ARGS__);                                                                          \
                                                                                                                       \
  /* 2) C++17 SFINAE: does s.__async__func_name(args...) compile? (defaults allowed) */                                \
  template <class S, class... A>                                                                                       \
  static auto __async__test_invocable__##func_name(int)->decltype(                                                     \
    std::declval<S&>().__async__##func_name(std::declval<A>()...), std::true_type{});                                  \
  template <class, class...> static auto __async__test_invocable__##func_name(...)->std::false_type;                   \
                                                                                                                       \
public:                                                                                                                \
  template <class... Args> CppUtil::Promise<ret_type> func_name(Args&&... a)                                           \
  {                                                                                                                    \
    using Self = std::remove_reference_t<decltype(*this)>;                                                             \
    static_assert(decltype(__async__test_invocable__##func_name<Self, Args...>(0))::value,                             \
                  "Function '" #ret_type " " #func_name "(" #__VA_ARGS__ ")' is not invocable with given arguments");  \
    return CppUtil::Async::async<ret_type>([this](auto&&... xs) -> ret_type                                            \
                                           { return this->__async__##func_name(std::forward<decltype(xs)>(xs)...); },  \
                                           std::forward<Args>(a)...);                                                  \
  }                                                                                                                    \
  static_assert(true, "__async_member_decl__ requires a trailing semicolon")

// #define __async_member_decl__(ret_type, func_name, ...)                                                                 \
// private:                                                                                                                \
//   ret_type __async__##func_name(__VA_ARGS__);                                                                           \
//                                                                                                                         \
// public:                                                                                                                 \
//   template <typename... Args> CppUtil::Promise<ret_type> func_name(Args&&... a)                                         \
//   {                                                                                                                     \
//     using Self = std::remove_reference_t<decltype(*this)>;                                                              \
//     static_assert(requires (Self& s, auto&&... as) {{ s.__async__##func_name(std::forward<decltype(as)>(as)...) }-> std::convertible_to<ret_type>;},                           \
//                   "Function: " #ret_type " " #func_name "(" #__VA_ARGS__                                                \
//                   ") is not invocable with given arguments; see compiler log for more info!");                          \
//     return CppUtil::Async::async<ret_type>(                                                                             \
//       [this](auto&&... xs) -> ret_type { return this->__async__##func_name(std::forward<decltype(xs)>(xs)...); }, std::forward<Args>(a)...);                              \
//   }                                                                                                                     \
//                                                                                                                         \
//   static_assert(true, "This is so you can end the line with a ';' ;)")

// Implementation of an async function
// Add your implementation of `__async__func_name` (will be called asynchronously on call of `func_name`) after the macro.
// Will also create the implementation of the public member function `func_name`.
// Must be placed outside the classes body.
#define __async_member_impl__(ret_type, class_name, func_name, ...)                                                    \
  ret_type class_name ::__async__##func_name(__VA_ARGS__)

// Call the async function and await its return
#define __await__(func_name, ...) func_name(__VA_ARGS__).get()
} // namespace CppUtil
