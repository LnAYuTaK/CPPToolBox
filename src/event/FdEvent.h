#pragma once

#include <sys/epoll.h>
#include <functional>
#include "Event.h"
class FdEvent : public Event {
 public:
  using Event::Event;
  virtual bool init(int fd, Mode mode) = 0;
  virtual ~FdEvent() {}
};
