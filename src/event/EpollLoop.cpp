#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <vector>

#include "CLog.h"
#include "MacroDef.h"

#include "EpollFdEvent.h"
#include "EpollLoop.h"
#include "EpollPoller.h"

EpollLoop::EpollLoop() 
              :keepRunning_(true)
              ,eventHandling_(false)
              ,callingPendingFunctors_(false)
              // , iteration_(0)
              ,poller_(std::make_unique<EpollPoller>(this))
              ,currentActiveEvent_(nullptr)
{

}

EpollLoop::~EpollLoop()
{
  int fd = poller_->epollFd();
  if(fd<0)
  {
     ::close(fd);
  }
}
 int EpollLoop::epollFd() const 
{
    if(poller_!=nullptr)
    {
      return poller_->epollFd();
    }
    else
    {
      CLOG_ERROR()<< "Epoll Get Fd error";
      return -1;
    }
}

bool EpollLoop::isInLoopThread() { return false; }

bool EpollLoop::isRunning() const { return false; }

void EpollLoop::runLoop(Mode mode) {

  if (epollFd() < 0) return;
  keepRunning_ = (mode == Loop::Mode::kForever);
  do {
    activeEvents_.clear();
    //handleNextFunc();  //处理LOOP池子事件
    poller_->poll(10000,&activeEvents_);
    eventHandling_ = true;
    for (EpollFdEvent* fdEvent : activeEvents_)
    {
      currentActiveEvent_ =  fdEvent;
      currentActiveEvent_->handleEvent(50);
    }
    eventHandling_ = false;
    } while (keepRunning_);
}

void EpollLoop::updateEvent(EpollFdEvent* event)
{
  assert(event->getLoop() == this);
  poller_->updateEvent(event);
}

void EpollLoop::removeEvent(EpollFdEvent* event)
{
  assert(event->getLoop() == this);
  if (eventHandling_)
  {
    assert(currentActiveEvent_ == event ||
      std::find(activeEvents_.begin(),  \
       activeEvents_.end(), event)  \
       == activeEvents_.end()); \
  }
  poller_->removeEvent(event);
}

bool EpollLoop::hasEvent(EpollFdEvent* event)
{
  assert(event->getLoop() == this);
  return poller_->hasEvent(event);
}

void EpollLoop::exitLoop(const std::chrono::milliseconds &wait_time) {
  keepRunning_ = false;
}

EpollFdEvent *EpollLoop::creatFdEvent(const std::string &fdName) {
  return new EpollFdEvent(this, fdName);
}
