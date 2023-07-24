#include <sys/epoll.h>
#include <unistd.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

#include "CLog.h"
#include "MacroDef.h"

#include "EpollLoop.h"
#include "EpollFdEvent.h"
#include "TimerEvent.h"
#include "EpollPoller.h"
#include "SystemInfo.h"
#define MAX_THREAD_NUM   5
EpollLoop::EpollLoop()
    :keepRunning_(true)
    ,eventHandling_(false)
    ,callingPendingFunctors_(false)
    ,poller_(std::make_unique<EpollPoller>(this))
    ,currentActiveEvent_(nullptr) 
    ,taskQueue_(std::make_unique<TaskQueue>())
    ,threadPool_(std::make_unique<ThreadPool>(MAX_THREAD_NUM))
      {}
EpollLoop::~EpollLoop() {
  int fd = poller_->epollFd();
  if (fd < 0) {
    ::close(fd);
  }
}

int EpollLoop::epollFd() const {
  if (poller_ != nullptr) {
    return poller_->epollFd();
  } else {
    CLOG_ERROR() << "Epoll Get Fd error";
    return -1;
  }
}

bool EpollLoop::isInLoopThread() { return false; }

bool EpollLoop::isRunning() const { return keepRunning_; }

void  EpollLoop::runTask (Task &&fun,bool runInThreadPool) {
  if(!runInThreadPool) {
      this->taskQueue_->emplace_back(std::move(fun));
  }
  else{
      this->threadPool_->submit(std::move(fun));
  }
}

void EpollLoop::handleTaskFun(){
 std::lock_guard<std::recursive_mutex> g(lock_);
  if(taskQueue_->size()>0){
      Task &fun  = taskQueue_->front();  // 获取第一个元素
       taskQueue_->pop_front();  // 移除第一个元素
       fun();
  }
}

void EpollLoop::runLoop(Mode mode) {
  if (epollFd() < 0) return;
  keepRunning_ = (mode == Loop::Mode::kForever);
  threadPool_->start();
  do {
    activeEvents_.clear();
    poller_->poll(0, &activeEvents_);
    //Handle Task
    handleTaskFun();
    // eventHandling_ = true;
    for (EpollFdEvent* fdEvent : activeEvents_) {
      currentActiveEvent_ = fdEvent;
      currentActiveEvent_->handleEvent(50);
    }
    // eventHandling_ = false;
  } while (keepRunning_);
  keepRunning_ = false;
}

void EpollLoop::updateEvent(EpollFdEvent* event) {
  assert(event->getLoop() == this);
  poller_->updateEvent(event);
}

void EpollLoop::removeEvent(EpollFdEvent* event) {
  assert(event->getLoop() == this);
  if (eventHandling_) {
    assert(currentActiveEvent_ == event ||
           std::find(activeEvents_.begin(), activeEvents_.end(), event) ==
               activeEvents_.end());
  }
  poller_->removeEvent(event);
}

bool EpollLoop::hasEvent(EpollFdEvent* event) {
  assert(event->getLoop() == this);
  return poller_->hasEvent(event);
}

void EpollLoop::exitLoop(const std::chrono::milliseconds& waitTime) {
  keepRunning_ = false;
}

EpollFdEvent* EpollLoop::creatFdEvent(const std::string& eventName) {
  return new EpollFdEvent(this, eventName);
}

TimerEvent* EpollLoop::creatTimerEvent(const std::string& eventName){
  return new TimerEvent(this, eventName);
}
