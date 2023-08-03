
#include "Poller.h"
#include "EpollFdEvent.h"
#include "Loop.h"

#include <assert.h>
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "CLog.h"
#include "MacroDef.h"
#include "Utils.h"

namespace {
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}  // namespace

#ifndef DEFAULT_MAX_LOOP_ENTRIES
#define DEFAULT_MAX_LOOP_ENTRIES (256)
#endif

Poller::Poller(Loop* loop)
    : epollFd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(DEFAULT_MAX_LOOP_ENTRIES) {
  if (epollFd_ < 0) {
    CLOG_ERROR() << "Epoller Create EpollFd Error";
  }
}

Poller::~Poller() { CHECK_CLOSE_RESET_FD(epollFd_); }

int Poller::poll(int timeoutMs, FdEventList* activeEvents) {
  int numEvents = ::epoll_wait(epollFd_, &*events_.begin(),
                               static_cast<int>(events_.size()), timeoutMs);
  int savedErrno = errno;
  if (numEvents > 0) {
    fillActiveEvents(numEvents, activeEvents);
    if (static_cast<size_t>(numEvents) == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numEvents == 0) {
  } else {
    if (savedErrno != EINTR) {
      errno = savedErrno;
    }
  }
  return 1;
}

void Poller::fillActiveEvents(int numEvents,
                                   FdEventList* activeEvents) const {
  assert(static_cast<size_t>(numEvents) <= events_.size());
  for (int i = 0; i < numEvents; ++i) {
    EpollFdEvent* fdEvent = static_cast<EpollFdEvent*>(events_[i].data.ptr);
    int fd = fdEvent->fd();
    auto it = fdEventMaps_.find(fd);
    assert(it != fdEventMaps_.end());
    assert(it->second == fdEvent);
    fdEvent->setRevents(events_[i].events);
    activeEvents->push_back(fdEvent);
  }
}

void Poller::updateEvent(EpollFdEvent* fdEvent) {
  const int index = fdEvent->index();
  //如果是新增加事件或者已经删除事件
  if (index == kNew || index == kDeleted) {
    // a new one, add with EPOLL_CTL_ADD
    int fd = fdEvent->fd();
    //这里做一个map元素检查保证存在元素
    if (index == kNew) {
      assert(fdEventMaps_.find(fd) == fdEventMaps_.end());
      fdEventMaps_[fd] = fdEvent;
    } else {
      assert(fdEventMaps_.find(fd) != fdEventMaps_.end());
      assert(fdEventMaps_[fd] == fdEvent);
    }
    fdEvent->setIndex(kAdded);
    update(EPOLL_CTL_ADD, fdEvent);
  } else {
    //这里是对原有事件更新
    int fd = fdEvent->fd();
    assert(fdEventMaps_.find(fd) != fdEventMaps_.end());
    assert(fdEventMaps_[fd] == fdEvent);
    assert(index == kAdded);
    if (fdEvent->isNoneEvent()) {
      update(EPOLL_CTL_DEL, fdEvent);
      fdEvent->setIndex(kDeleted);
    } else {
      update(EPOLL_CTL_MOD, fdEvent);
    }
  }
}

void Poller::removeEvent(EpollFdEvent* fdEvent) {
  int fd = fdEvent->fd();
  assert(fdEventMaps_.find(fd) != fdEventMaps_.end());
  assert(fdEventMaps_[fd] == fdEvent);
  assert(fdEvent->isNoneEvent());
  int index = fdEvent->index();
  assert(index == kAdded || index == kDeleted);
  size_t n = fdEventMaps_.erase(fd);
  assert(n == 1);
  if (index == kAdded) {
    update(EPOLL_CTL_DEL, fdEvent);
  }
  fdEvent->setIndex(kNew);
}

void Poller::update(int operation, EpollFdEvent* fdEvent) {
  struct epoll_event event;
  Utils::memZero(&event, sizeof event);
  event.events = fdEvent->events();
  event.data.ptr = fdEvent;
  int fd = fdEvent->fd();
  if (::epoll_ctl(epollFd_, operation, fd, &event) < 0) {
    if (operation == EPOLL_CTL_DEL) {
      CLOG_ERROR() << fdEvent->name() << ": Update  Epoll Error";
    } else {
      CLOG_ERROR() << fdEvent->name() << ": Update  Epoll Error";
    }
  }
}
bool Poller::hasEvent(EpollFdEvent* event) const {
  FdEvenMap::const_iterator it = fdEventMaps_.find(event->fd());
  return it != fdEventMaps_.end() && it->second == event;
}
