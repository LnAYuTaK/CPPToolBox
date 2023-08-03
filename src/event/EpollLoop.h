#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "FdEvent.h"
#include "Loop.h"
#include "MacroDef.h"
#include "ThreadPool.h"

class EpollPoller;
class EpollFdEvent;
class EpollLoop : public Loop {

 public:
  using FdEventList = std::vector<EpollFdEvent*>;
  using PollerPtr = std::unique_ptr<EpollPoller>;
  using ThreadPoolPtr = std::unique_ptr<ThreadPool>;
  using Task = std::function<void()>;
  using TaskQueue = std::deque<Task>;
  using TaskQueuePtr = std::unique_ptr<TaskQueue>;

  explicit EpollLoop();
  
  ~EpollLoop() override;
  /**
   * @brief 运行loop
   *
   * @param mode
   */
  void runLoop(Mode mode = Mode::kForever) override;
  /**
   * @brief waittime 后关闭loop
   *
   * @param wait_time
   */
  void exitLoop(const std::chrono::milliseconds& wait_time) override;
  /**
   * @brief 判断是否在loop主线程
   *
   * @return true
   * @return false
   */
  bool isInLoopThread() override;
  /**
   * @brief 判断loop是否在运行状态
   *
   * @return true
   * @return false
   */
  bool isRunning() const override;
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
  EpollFdEvent* creatFdEvent(const std::string& eventName) override;
  /**
   * @brief  创建定时器事件
   *
   * @param eventName
   * @return TimerEvent*
   */
  TimerEvent* creatTimerEvent(const std::string& eventName) override;
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
