/**
 * @file SafeQueue.h
 * @author 刘宽 (807874484@qq.com)
 * @brief  线程安全队列
 * @version 0.1
 * @date 2023-06.5
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <mutex>
#include <queue>

template <typename T>
class SafeQueue {
 private:
  std::queue<T> m_queue;
  std::mutex m_mutex;

 public:
  SafeQueue() {}

  SafeQueue(SafeQueue& other) {
    // TODO:
  }

  ~SafeQueue() {}
  //判空
  bool empty() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  }
  //大小
  int size() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
  }
  //入队
  void enqueue(T& t) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(t);
  }
  //出队
  bool dequeue(T& t) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
      return false;
    }
    t = std::move(m_queue.front());

    m_queue.pop();
    return true;
  }
};