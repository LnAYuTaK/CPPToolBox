/**
 * @file Buffer.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief Thread缓存区模板
 * @version 0.1
 * @date 2023-07-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <atomic>
#include <memory>

template <typename T>
class Buffer {
 public:
  Buffer() : back(new T), middle(new T), front(new T), queued(false) {}

  void put(const T &data) {
    *back = data;
    std::shared_ptr<T> previous = std::atomic_exchange(&middle, back);
    std::atomic_store(&back, previous);

    queued = true;
  }

  bool get(T &data) {
    if (!std::atomic_exchange(&queued, false)) {
      return false;
    }
    std::shared_ptr<T> previous = std::atomic_exchange(&middle, front);
    std::atomic_store(&front, previous);

    data = *front;

    return true;
  }

 private:
  std::shared_ptr<T> back, middle, front;
  std::atomic<bool> queued;
};
