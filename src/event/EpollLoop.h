#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "FdEvent.h"
#include "Loop.h"
#include "MacroDef.h"

class EpollPoller;
class EpollFdEvent;
class EpollLoop : public Loop {

  DISALLOW_COPY_AND_ASSIGN(EpollLoop)
  
  using FdEventList = std::vector<EpollFdEvent*>;
  using PollerPtr   = std::unique_ptr<EpollPoller>;
  
 public:
  explicit EpollLoop();
  ~EpollLoop() override;
  // 是否与Loop在同一个线程内TUDO
  bool isInLoopThread() override;
  // Loop是否正在运行
  bool isRunning() const override;
  //开始运行EPoll循环
  void runLoop(Mode mode = Mode::kForever) override;
  //创建普通的FD读写事件
  EpollFdEvent* creatFdEvent(const std::string& eventName) override;

  // EpollFdEvent* creatTimerEvent(const std::string& eventName) override;

  //Exit Loop 
  void exitLoop(const std::chrono::milliseconds& wait_time) override;
  //Get EpollFD;
  inline int epollFd() const;

  void updateEvent(EpollFdEvent* event);
  void removeEvent(EpollFdEvent* event);
  bool hasEvent(EpollFdEvent* event);

 private:
  bool keepRunning_;
  FdEventList activeEvents_;
  EpollFdEvent* currentActiveEvent_;
  bool eventHandling_;          /* atomic */
  bool callingPendingFunctors_; /* atomic */
  // int64_t iteration_;
  PollerPtr poller_;
};
