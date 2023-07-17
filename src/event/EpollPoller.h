
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
  //Poller启动Epoll池
  int poll(int timeoutMs, FdEventList* activeEvents);
  //更新事件
  void updateEvent(EpollFdEvent*  fdEvent);
  //移除事件
  void removeEvent(EpollFdEvent* fdEvent);
 //判断是否拥有事件
  bool hasEvent(EpollFdEvent* event) const;
  //Epoll的文件描述符
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
