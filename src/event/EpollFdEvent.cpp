#include <sys/epoll.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

#include "EpollFdEvent.h"
#include "FdEvent.h"

const int EpollFdEvent::kNoneEvent = 0;
const int EpollFdEvent::kReadEvent = EPOLLIN | EPOLLPRI;
const int EpollFdEvent::kWriteEvent = EPOLLOUT;

EpollFdEvent::EpollFdEvent(EpollLoop *wpLoop, const std::string &name)
    : FdEvent(name),
      loop_(wpLoop),
      addedToLoop_(false),
      isStopAfterTrigger_(false),
      revents_(0),
      index_(-1),
      events_(0),
      isEnabled_(false),
      eventHandling_(false),
      lockEvent_(false) {}
EpollFdEvent::~EpollFdEvent() { CHECK_CLOSE_RESET_FD(fd_); }

void EpollFdEvent::update() {
  addedToLoop_ = true;
  loop_->updateEvent(this);
}

void EpollFdEvent::remove() {
  assert(isNoneEvent());
  addedToLoop_ = false;
  loop_->removeEvent(this);
}

bool EpollFdEvent::init(int fd, Mode mode) {
  fd_ = fd;
  if (mode == FdEvent::Mode::kOneshot) {
    isStopAfterTrigger_ = true;
  }
  return true;
}

void EpollFdEvent::handleEvent(int time) {
  if (lockEvent_ == false) {
    handleLockEvent(time);
  }
}
void EpollFdEvent::handleLockEvent(int receiveTime) {
  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
    if (closeCallback_) closeCallback_();
  }
  if (revents_ & EPOLLERR) {
    CLOG_WARN() << name() << ": EpollError";
  }
  if (revents_ & (EPOLLERR | EPOLLERR)) {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
    // Read
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revents_ & EPOLLOUT) {
    if (writeCallback_) writeCallback_();
  }
  if (isStopAfterTrigger_) {
    lockEvent_ = true;
    this->disableAll();
  }
  //记录时间//
}
