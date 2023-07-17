#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "Loop.h"
#include "FdEvent.h"

class  EpollPoller;
class EpollFdEvent;
class EpollLoop : public Loop {
  
  using FdEventList   = std::vector<EpollFdEvent*> ;
  using PollerPtr     = std::unique_ptr<EpollPoller>;
 public:
  explicit EpollLoop();
  virtual ~EpollLoop() override;
  // 是否与Loop在同一个线程内
  virtual bool isInLoopThread() override;
  // Loop是否正在运行
  virtual bool isRunning() const override;
  //开始运行EPoll循环
  virtual void runLoop(Mode mode) override;
  //创建普通的FD读写事件
  virtual EpollFdEvent *creatFdEvent(const std::string &fdName) override;

  virtual void exitLoop(const std::chrono::milliseconds &wait_time) override;

  inline int epollFd() const ;

  void updateEvent(EpollFdEvent* event);
  void removeEvent(EpollFdEvent* event);
  bool hasEvent(EpollFdEvent* event);
 private:
  bool keepRunning_ ;
  FdEventList activeEvents_;
  EpollFdEvent* currentActiveEvent_;
  bool eventHandling_; /* atomic */
  bool callingPendingFunctors_; /* atomic */
  // int64_t iteration_;
  PollerPtr  poller_;
};
