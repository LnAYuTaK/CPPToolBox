#pragma once

#include <sys/epoll.h>
#include "Fd.h"
#include "FdEvent.h"
class EpollLoop;
class EpollFdEvent : public FdEvent {
 public:
  explicit EpollFdEvent(EpollLoop *wp_loop, const std::string &name);
  virtual ~EpollFdEvent() override;

  using EventCallback = std::function<void()>;
  using ReadEventCallback = std::function<void(int)>;

 public:
   //初始化Fdevent
  bool initialize(int fd, short events, Mode mode) override;

  void setReadCallback(ReadEventCallback &&cb) {
    readCallback_ = std::move(cb);
  }
   void setWriteCallback(EventCallback &&cb) {
    writeCallback_ = std::move(cb);
  }
  void setCloseCallback(EventCallback &&cb) {
    closeCallback_ = std::move(cb);
  }
  void setErrorCallback(EventCallback &&cb) {
    errorCallback_ = std::move(cb);
  }

  void handleEvent(int time);

  void enableReading() { events_ |= kReadEvent; update(); }
  void enableWriting() { events_ |= kWriteEvent; update(); }
  void disableReading() { events_ &= ~kReadEvent; update(); }
  void disableWriting() { events_ &= ~kWriteEvent; update(); }
  void disableAll() { events_ = kNoneEvent; update(); }
  bool isNoneEvent() const { return events_ == kNoneEvent; }

  EpollLoop *getLoop(){return this->loop_;}

  const int fd() const { return fd_->get(); }

  int index() { return index_; }
  void setIndex(int idx) { index_ = idx; }
  void setRevents(int revt) {revents_ = revt; }
  int events() const { return events_; }

 private:
  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;
  //EPool循环池
  EpollLoop * loop_;
  //持有的文件描述符
  Fd *fd_;
  //是否已经加入到了Loop池
  bool  addedToLoop_;
  //是否只执行一次事件
  bool isStopAfterTrigger_ ;
   //Epool 中接受的事件类型
  int  revents_    ;
  //标号
  int  index_; 
   //事件类型
  short events_ ;
  //是否时能
  bool isEnabled_;
  //是否正在处理事件
  bool eventHandling_;
  //通用回调事件
  CallbackFunc cb_;
  //读回调事件
  ReadEventCallback readCallback_;
  //写回调事件
  EventCallback writeCallback_;
  //关闭事件回调
  EventCallback closeCallback_;
  //错误事件回调
  EventCallback errorCallback_;
  //更新事件
  void update();
  //删除事件
  void remove();
  //处理保护事件
  void handleLockEvent(int receiveTime);

};
