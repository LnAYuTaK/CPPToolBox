#pragma once
/**
 * @file ThreadPool.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  线程池
 * @version 0.1
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include "SafeQueue.h"
#include "MacroDef.h"

class ThreadPool {
  DISALLOW_COPY_AND_ASSIGN(ThreadPool);
 private:
  class ThreadWorker {
   public:
    ThreadWorker(ThreadPool *pool, const int id) : m_pool(pool), m_id(id) {}
    void operator()() {
      std::function<void()> func;
      bool dequeued;
      while ((!m_pool->m_shutdown)&&(m_pool->isRunning())) {
        {
          std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
          if (m_pool->m_queue.empty()) {
            m_pool->m_conditional_lock.wait(lock);
          }
          dequeued = m_pool->m_queue.dequeue(func);
        }
        if (dequeued) {
          func();
        }
      }
    }
    private:
    int m_id;
    ThreadPool *m_pool = nullptr;
  };

 public:

  // ThreadPool(const int n_threads)
  //     : m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false) {
  //   for (size_t i = 0; i < m_threads.size(); ++i) {
  //     m_threads[i] = std::thread(ThreadWorker(this, i));
  //   }
  // }
   
  ThreadPool(const int maxThreadNum)
          :maxThreadNum_(maxThreadNum)
          ,m_shutdown(false)
          ,startFlag(false){
  }
  void start(){
      startFlag = true;
  }

  bool isRunning()const{
     return startFlag;
  }

  //结束关闭所有线程
  void shutdown() {
    m_shutdown = true;
    startFlag  = false;
    m_conditional_lock.notify_all();

    for (size_t i = 0; i < m_threads.size(); ++i) {
      if (m_threads[i].joinable()) {
        m_threads[i].join();
      }
    }
  }
  //提交一个线程任务异步执行
  template <typename F, typename... Args>
  auto submit(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
    if(m_threads.size()+1 <= maxThreadNum_) {
      m_threads.push_back(std::move(std::thread(ThreadWorker(this,(int)m_threads.size()+1))));
    }
    std::function<decltype(f(args...))()> func =
        std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto task_ptr =
        std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
    std::function<void()> wrapper_func = [task_ptr]() { (*task_ptr)(); };

    m_queue.enqueue(wrapper_func);

    m_conditional_lock.notify_one();

    return task_ptr->get_future();
  }
  private:
    int maxThreadNum_;
    bool m_shutdown;
    bool startFlag;
    SafeQueue<std::function<void()>> m_queue;
    std::vector<std::thread> m_threads;
    std::mutex m_conditional_mutex;
    std::condition_variable m_conditional_lock;

};
