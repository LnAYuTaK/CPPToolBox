
#pragma once
#include <map>
#include <vector>
#include <unordered_map>
#include "EpollLoop.h"

struct epoll_event;
class EpollFdEvent;
class EpollPoller {

  using FdEventList = std::vector<EpollFdEvent*> ;
  using EventList   = std::vector<struct epoll_event>;
  using FdEvenMap   = std::unordered_map<int, EpollFdEvent*>;

 public:
  EpollPoller(EpollLoop* loop);
  ~EpollPoller();

  int poll(int timeoutMs, FdEventList* activeEvents);
  
  void updateEvent(EpollFdEvent*  fdEvent);

  void removeEvent(EpollFdEvent* fdEvent);

  bool hasEvent(EpollFdEvent* event) const;

  inline int epollFd(){return epollFd_;}

 private:
  void fillActiveEvents(int numEvents,
                          FdEventList* activeEvents) const;

  void update(int operation, EpollFdEvent* fdEvent);
  int epollFd_;
  FdEvenMap fdEventMaps_;
  EventList events_;
  EpollLoop * loop_;
};
