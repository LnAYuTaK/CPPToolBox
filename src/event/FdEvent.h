#pragma once

#include <sys/epoll.h>
#include <functional>
#include "Event.h"
class FdEvent : public Event {
 public:
  using CallbackFunc = std::function<void(short events)>;
  using Event::Event;
  virtual bool initialize(int fd, short events, Mode mode) = 0;
  // virtual void setCallback(CallbackFunc &&cb) = 0;

 public:
  virtual ~FdEvent() {}
};
