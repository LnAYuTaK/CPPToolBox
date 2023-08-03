#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <functional>
#include <string>
#include <vector>

#include "MacroDef.h"
#include "ThreadPool.h"

class FdEvent;
class EpollFdEvent;
class TimerEvent;
class Poller;
class Loop {
 public:
   enum class Mode {
    kOnce,    //!< 仅执行一次
    kForever  //!< 一直执行
  };
  using FdEventList = std::vector<EpollFdEvent*>;
  using PollerPtr = std::unique_ptr<Poller>;
  using ThreadPoolPtr = std::unique_ptr<ThreadPool>;
  using Task = std::function<void()>;
  using TaskQueue = std::deque<Task>;
  using TaskQueuePtr = std::unique_ptr<TaskQueue>;

  static Loop *New();
  
  explicit Loop();
  
  ~Loop() ;
  /**
   * @brief 运行loop
   *
   * @param mode
   */
  void runLoop(Mode mode = Mode::kForever) ;
  /**
   * @brief waittime 后关闭loop
   *
   * @param wait_time
   */
  void exitLoop(const std::chrono::milliseconds& wait_time) ;
  /**
   * @brief 判断是否在loop主线程
   *
   * @return true
   * @return false
   */
  bool isInLoopThread() ;
  /**
   * @brief 判断loop是否在运行状态
   *
   * @return true
   * @return false
   */
  bool isRunning() const ;
  /**
   * @brief  创建任务
   *
   * @param fun
   * @param runInThreadPool
   */
  void runTask(Task&& fun, bool runInThreadPool);
  /**
   * @brief 处理任务队列任务
   *
   */
  void handleTaskFun();
  /**
   * @brief 创建普通Fd事件任务
   *
   * @param eventName
   * @return EpollFdEvent*
   */
  EpollFdEvent* CreatFdEvent(const std::string& eventName) ;
  /**
   * @brief  创建定时器事件
   *
   * @param eventName
   * @return TimerEvent*
   */
  TimerEvent* CreatTimerEvent(const std::string& eventName) ;
  /**
   * @brief 获取Epoll的文件描述符
   *
   * @return int
   */
  inline int epollFd() const;
  /**
   * @brief 更新事件任务
   *
   * @param event
   */
  void updateEvent(EpollFdEvent* event);
  /**
   * @brief 移除事件任务
   *
   * @param event
   */
  void removeEvent(EpollFdEvent* event);
  /**
   * @brief 判断事件是否在loop里
   *
   * @param event
   * @return true
   * @return false
   */
  bool hasEvent(EpollFdEvent* event);

 private:
  bool keepRunning_;
  bool eventHandling_;
  bool callingPendingFunctors_;

  PollerPtr poller_;
  FdEventList activeEvents_;
  EpollFdEvent* currentActiveEvent_;
  TaskQueuePtr taskQueue_;
  ThreadPoolPtr threadPool_;
  std::recursive_mutex lock_;
};
