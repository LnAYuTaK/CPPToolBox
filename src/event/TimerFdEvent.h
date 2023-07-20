#pragma once 

#include <string>
#include <chrono>
#include <functional>
#include "FdEvent.h"
#include "EpollFdEvent.h"
#include "EpollLoop.h"
#include "MacroDef.h"

class TimerFdEvent : public Event {

    using Callback = std::function<void ()>;

 public:
    explicit TimerFdEvent(EpollLoop *loop, const std::string &name);
    virtual ~TimerFdEvent()override ;

 public:
    bool init(const std::chrono::nanoseconds first,
                    const std::chrono::nanoseconds repeat = std::chrono::nanoseconds::zero());
    /// 清理
    void cleanup();
    //
    bool start();
    //Stop
    bool stop();
    // 获取距下一次触发剩余的时长
    std::chrono::nanoseconds remainTime() const;
  private:
    int timerFd_;
    
    struct itimerspec timerSpec_;
    void onTimerEvent();
    EpollFdEvent *timerFdEvent_;
};

