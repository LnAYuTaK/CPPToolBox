#include "TimerFdEvent.h"
//C
#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>
//CPP
#include <chrono>

namespace {
constexpr auto NANOSECS_PER_SECOND = 1000000000ul;
}

TimerFdEvent::TimerFdEvent(EpollLoop *loop, const std::string &name)
                            :Event(name)
                            ,timerFd_(-1) {
  timerFdEvent_ = loop->creatFdEvent(name);
}

TimerFdEvent::~TimerFdEvent(){
}

bool TimerFdEvent::init(const std::chrono::nanoseconds first, 
                         const std::chrono::nanoseconds repeat) {
    timerFd_ = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerFd_ < 0){
        return false;
    }
    auto first_nanosec = first.count();
    auto repeat_nanosec = repeat.count();

    timerSpec_.it_value.tv_sec = first_nanosec / NANOSECS_PER_SECOND;
    timerSpec_.it_value.tv_nsec = first_nanosec % NANOSECS_PER_SECOND;
    timerSpec_.it_interval.tv_sec = repeat_nanosec / NANOSECS_PER_SECOND; 
    timerSpec_.it_interval.tv_nsec = repeat_nanosec % NANOSECS_PER_SECOND;

    auto mode = repeat_nanosec != 0 ? Event::Mode::kPersist : Event::Mode::kOneshot;
    timerFdEvent_->init(timerFd_,mode);
    return true;
}

void TimerFdEvent::cleanup() {
    CHECK_CLOSE_RESET_FD(timerFd_);
}

bool TimerFdEvent::start() {
    timerFdEvent_->enableReading();
    if (::timerfd_settime(timerFd_, TFD_TIMER_CANCEL_ON_SET, &timerSpec_, NULL) < 0) {
        return false;
    }
    return true;
}

bool TimerFdEvent::stop() {
    struct itimerspec ts = {0};
    if (::timerfd_settime(timerFd_, TFD_TIMER_CANCEL_ON_SET, &timerSpec_, NULL) < 0) {
        return false;
    }
    timerFdEvent_->disableReading();
    return true;
}

std::chrono::nanoseconds TimerFdEvent::remainTime() const {
    std::chrono::nanoseconds remain_time = std::chrono::nanoseconds::zero();
    struct itimerspec ts;
    int ret = ::timerfd_gettime(timerFd_, &ts);
    if (ret == 0) {
        remain_time += std::chrono::seconds(ts.it_value.tv_sec);
        remain_time += std::chrono::nanoseconds(ts.it_value.tv_nsec);
    } else {
    //    CLOG_ERROR() << name() << ": Get ReMainTime Error";
    }
    return remain_time;
}

void TimerFdEvent::onTimerEvent()
{
    // if (events & FdEvent::kReadEvent) {
    //     uint64_t exp = 0;
    //     int len  = ::read(timerFd_, &exp, sizeof(exp));
    //     if (len <= 0)
    //         return;

    //     if (d_->cb) {
    //         ++d_->cb_level;
    //         d_->cb();
    //         --d_->cb_level;
    //     }
    // }
}

