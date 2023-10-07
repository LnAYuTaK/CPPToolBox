/**
 * @file MacroDef.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  通用宏定义
 *
 * @version 0.1
 * @date 2023-07-11
 */

#pragma once
#include <unistd.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <new>
#include <thread>
#include <type_traits>
#include <utility>

#define DEFINE_TYPE_TRAIT(name, func)                      \
  template <typename T>                                    \
  struct name {                                            \
    template <typename Class>                              \
    static constexpr bool Test(decltype(&Class::func) *) { \
      return true;                                         \
    }                                                      \
    template <typename>                                    \
    static constexpr bool Test(...) {                      \
      return false;                                        \
    }                                                      \
                                                           \
    static constexpr bool value = Test<T>(nullptr);        \
  };                                                       \
                                                           \
  template <typename T>                                    \
  constexpr bool name<T>::value;

DEFINE_TYPE_TRAIT(HasShutdown, Shutdown)

template <typename T>
typename std::enable_if<HasShutdown<T>::value>::type CallShutdown(T *instance) {
  instance->Shutdown();
}

template <typename T>
typename std::enable_if<!HasShutdown<T>::value>::type CallShutdown(
    T *instance) {
  (void)instance;
}
//未定义 去Waring
#undef UNUSED
#undef DISALLOW_COPY_AND_ASSIGN
#define UNUSED(param) (void)param

//禁止拷贝
#define DISALLOW_COPY(classname)         \
  classname(const classname &) = delete; \
  classname &operator=(const classname &) = delete;

//禁止移动构造
#define DISALLOW_MOVE(classname)    \
  classname(classname &&) = delete; \
  classname &operator=(classname &&) = delete

//禁止拷贝构造宏
#define DISALLOW_COPY_AND_ASSIGN(classname)         \
  classname(const classname &) = delete;            \
  classname(const classname &&) = delete;           \
  classname &operator=(const classname &) = delete; \
  classname &operator=(classname &&) = delete;

//单例注册宏
#define DECLARE_SINGLETON(classname)                                      \
 public:                                                                  \
  static classname *Instance(bool create_if_needed = true) {              \
    static classname *instance = nullptr;                                 \
    if (!instance && create_if_needed) {                                  \
      static std::once_flag flag;                                         \
      std::call_once(flag,                                                \
                     [&] { instance = new (std::nothrow) classname(); }); \
    }                                                                     \
    return instance;                                                      \
  }                                                                       \
                                                                          \
  static void CleanUp() {                                                 \
    auto instance = Instance(false);                                      \
    if (instance != nullptr) {                                            \
      CallShutdown(instance);                                             \
    }                                                                     \
  }                                                                       \
                                                                          \
 private:                                                                 \
  classname();                                                            \
  DISALLOW_COPY_AND_ASSIGN(classname)

//当前线程等待毫秒级
#define THREAD_SLEEP_MS(x)                                     \
  do {                                                         \
    std::this_thread::sleep_for(std::chrono::milliseconds(x)); \
  } while (0)

//当前线程等待微妙级
#define THREAD_SLEEP_MCROS(x)                                  \
  do {                                                         \
    std::this_thread::sleep_for(std::chrono::microseconds(x)); \
  } while (0)

//! 获取固定数组成员个数
#define NUMBER_OF_ARRAY(arr) (sizeof(arr) / sizeof(*arr))

//! 资源释放相关宏
#define CHECK_DELETE_RESET_OBJ(obj) \
  do {                              \
    if (obj != nullptr) {           \
      delete obj;                   \
      obj = nullptr;                \
    }                               \
  } while (0)
#define CHECK_DELETE_OBJ(obj) \
  do {                        \
    if (obj != nullptr) {     \
      delete obj;             \
    }                         \
  } while (0)
#define CHECK_DELETE_RESET_ARRAY(arr) \
  do {                                \
    if (arr != nullptr) {             \
      delete[] arr;                   \
      arr = nullptr;                  \
    }                                 \
  } while (0)
#define CHECK_DELETE_ARRAY(arr) \
  do {                          \
    if (arr != nullptr) {       \
      delete[] arr;             \
    }                           \
  } while (0)
#define CHECK_FREE_RESET_PTR(ptr) \
  do {                            \
    if (ptr != nullptr) {         \
      free(ptr);                  \
      ptr = nullptr;              \
    }                             \
  } while (0)
#define CHECK_FREE_PTR(ptr) \
  do {                      \
    if (ptr != nullptr) {   \
      free(ptr);            \
    }                       \
  } while (0)
#define CHECK_CLOSE_RESET_FD(fd) \
  do {                           \
    if (fd != -1) {              \
      ::close(fd);               \
      fd = -1;                   \
    }                            \
  } while (0)
#define CHECK_CLOSE_FD(fd) \
  do {                     \
    if (fd != -1) {        \
      ::close(fd);         \
    }                      \
  } while (0)

#ifndef LIKELY
//大概率条件成立
#define LIKELY(x) __builtin_expect(!!(x), 1)
#endif

#ifndef UNLIKELY
//大概率条件不成立
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#endif

//基于无参构造、析构、swap,实现reset与移动函数
#define IMP_MOVE_RESET_FUNC_BASE_ON_SWAP(class_name)          \
  class_name::class_name(class_name &&other) { swap(other); } \
  class_name &class_name::operator=(class_name &&other) {     \
    if (this != &other) {                                     \
      reset();                                                \
      swap(other);                                            \
    }                                                         \
    return *this;                                             \
  }                                                           \
  void class_name::reset() {                                  \
    class_name tmp;                                           \
    swap(tmp);                                                \
  }
