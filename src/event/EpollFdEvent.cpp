#include <sys/epoll.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

#include "CLog.h"

#include "EpollFdEvent.h"
#include "EpollLoop.h"
#include "FdEvent.h"
#include "Loop.h"

const int EpollFdEvent::kNoneEvent = 0;
const int EpollFdEvent::kReadEvent = EPOLLIN | EPOLLPRI;
const int EpollFdEvent::kWriteEvent = EPOLLOUT;

EpollFdEvent::EpollFdEvent(EpollLoop *wpLoop, const std::string &name)
    : FdEvent(name)
    ,loop_(wpLoop)
    ,fd_(nullptr)
    ,addedToLoop_(false)
    ,isStopAfterTrigger_(false)
    ,revents_  (0)
    ,index_(-1)
    ,events_(0)
    ,isEnabled_(false)
    ,eventHandling_(false)
     {
     }

EpollFdEvent::~EpollFdEvent() {
  if(fd_!=nullptr)
  {
    fd_->close();
    delete fd_;
  }
}

void EpollFdEvent::update()
{
  addedToLoop_ = true;
  loop_->updateEvent(this);
}

void EpollFdEvent::remove()
{
  assert(isNoneEvent());
  addedToLoop_ = false;
  loop_->removeEvent(this);
}

bool EpollFdEvent::initialize(int fd, short events, Mode mode) {
  if (fd_ == nullptr) {
    fd_ = new Fd(fd);
  } else {
    CLOG_ERROR() << name() <<": Epoll Init Error";
    return false;
  }
  if (mode == FdEvent::Mode::kOneshot) {
    isStopAfterTrigger_ = true;
  }
  return true;
}

void EpollFdEvent::handleEvent(int time)
{
//Lock //
  handleLockEvent(time);
}

void EpollFdEvent::handleLockEvent(int receiveTime)
{
  //记录事件
  eventHandling_ = true;
  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
  {
    if (closeCallback_) closeCallback_();
  }
  if (revents_ & EPOLLERR)
  {
   CLOG_WARN()<< name() << ": EpollError";
  }
  if (revents_ & (EPOLLERR | EPOLLERR))
  {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
  {
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revents_ & EPOLLOUT)
  {
    if (writeCallback_) writeCallback_();
  }
  eventHandling_ = false;
  //记录时间//
}


