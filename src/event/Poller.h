
#pragma once

#include <map>
#include <unordered_map>
#include <vector>

#include "Loop.h"

struct epoll_event;
class Poller {
  using FdEventList = std::vector<EpollFdEvent*>;
  using EventList = std::vector<struct epoll_event>;
  using FdEvenMap = std::unordered_map<int, EpollFdEvent*>;

 public:
  Poller(Loop* loop);
  ~Poller();
  /**
   * @brief 启动Epoll
   *
   * @param timeoutMs
   * @param activeEvents
   * @return int
   */
  int poll(int timeoutMs, FdEventList* activeEvents);
  //更新事件
  void updateEvent(EpollFdEvent* fdEvent);
  //移除事件
  void removeEvent(EpollFdEvent* fdEvent);
  //判断是否拥有事件
  bool hasEvent(EpollFdEvent* event) const;
  // Epoll的文件描述符
  inline int epollFd() { return epollFd_; }

 private:
  //填充活跃事件
  void fillActiveEvents(int numEvents, FdEventList* activeEvents) const;
  //更新事件具体实现
  void update(int operation, EpollFdEvent* fdEvent);
  int epollFd_;
  FdEvenMap fdEventMaps_;
  EventList events_;
  Loop* loop_;
};
