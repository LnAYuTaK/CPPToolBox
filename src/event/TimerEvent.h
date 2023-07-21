/**
 * @file TimerEvent.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  计时器事件
 * @version 0.1
 * @date 2023-07-20
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <chrono>
#include <functional>
#include <string>
#include "EpollFdEvent.h"
#include "EpollLoop.h"
#include "FdEvent.h"
#include "MacroDef.h"

class TimerEvent : public Event {

  using TimerCallback = std::function<void()>;

 public:
  explicit TimerEvent(EpollLoop *loop, const std::string &name);
  virtual ~TimerEvent() override;

 public:
  /**
   * @brief   定时器时间初始化
   * 比如设置 first时间为当前时间  设置repeat为五秒,
   *  定时器会立刻触发记时五秒后触发事件然后重置重复,
   * 如果设置first为五秒,设置repeat五秒,就会五秒后触发计时
   * 五秒后重置总共十秒.
   * 如果只设置first 5秒就只会触发一次.
   * @param first 触发时间
   * @param repeat  间隔时间
   * @return true
   * @return false
   */
  bool init(
      const std::chrono::nanoseconds first,
      const std::chrono::nanoseconds repeat = std::chrono::nanoseconds::zero());

  void setTimerCallback(TimerCallback &&cb);
  /// 清理
  void cleanup();
  /**
   * @brief 启动定时器
   *
   * @return true
   * @return false
   */
  bool start();
  /**
   * @brief 获取定时器剩余时间
   *
   * @return std::chrono::nanoseconds
   */
  bool stop();
  // 获取距下一次触发剩余的时长
  std::chrono::nanoseconds remainTime() const;

 private:
  void onTimerEvent();
  int timerFd_;
  TimerCallback  timerCallback_;
  struct itimerspec timerSpec_;
  EpollFdEvent *TimerEvent_;
};
