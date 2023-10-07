#pragma once

#include <sys/epoll.h>

#include "Event.h"
#include "Fd.h"
#include "Loop.h"
namespace gwcpp
{

class EpollFdEvent : public Event {
 public:
  explicit EpollFdEvent(Loop *wp_loop, const std::string &name);
  virtual ~EpollFdEvent() override;

  using EventCallback = std::function<void()>;
  using ReadEventCallback = std::function<void(int)>;
  using ptr     = std::unique_ptr<EpollFdEvent>;
 public:
  /**
   * @brief 初始化EpollFd事件
   *
   * @param fd
   * @param mode
   * @return true
   * @return false
   */
  bool init(int fd, Mode mode);
  void setReadCallback(ReadEventCallback &&cb) {
    readCallback_ = std::move(cb);
  }
  void setWriteCallback(EventCallback &&cb) { writeCallback_ = std::move(cb); }
  void setCloseCallback(EventCallback &&cb) { closeCallback_ = std::move(cb); }
  void setErrorCallback(EventCallback &&cb) { errorCallback_ = std::move(cb); }
  /**
   * @brief 处理事件
   *
   * @param time
   */
  void handleEvent(int time);
  /**
   * @brief 时能读取
   *
   */
  void enableReading() {
    events_ |= kReadEvent;
    update();
  }
  /**
   * @brief 时能写入
   *
   */
  void enableWriting() {
    events_ |= kWriteEvent;
    update();
  }
  /**
   * @brief 关闭读事件
   *
   */
  void disableReading() {
    events_ &= ~kReadEvent;
    update();
  }
  /**
   * @brief 关闭写事件
   *
   */
  void disableWriting() {
    events_ &= ~kWriteEvent;
    update();
  }
  /**
   * @brief 关闭所有事件
   *
   */
  void disableAll() {
    events_ = kNoneEvent;
    update();
  }
  /**
   * @brief 是否是空事件
   *
   */
  bool isNoneEvent() const { return events_ == kNoneEvent; }
  bool isWriting() const { return events_ & kWriteEvent; }
  bool isReading() const { return events_ & kReadEvent; }

  Loop *getLoop() { return this->loop_; }
  const int fd() const { return fd_; }
  int  index() { return index_; }
  void setIndex(int idx) { index_ = idx; }
  void setRevents(int revt) { revents_ = revt; }
  int  events() const { return events_; }
  //删除当前的Epoll
  void remove();

 private:
  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;
  // EPool循环池
  Loop *loop_;
  //持有的文件描述符
  int fd_;
  //是否已经加入到了Loop池
  bool addedToLoop_;
  //是否只执行一次事件
  bool isStopAfterTrigger_;
  // Epool 中接受的事件类型
  int revents_;
  //标号
  int index_;
  //事件类型
  short events_;
  //是否时能
  bool isEnabled_;
  //是否正在处理事件
  bool eventHandling_;
  //是否执行保护事件
  bool lockEvent_;
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
  //处理保护事件
  void handleLockEvent(int receiveTime);
}; 
} // namespace gwcpp
