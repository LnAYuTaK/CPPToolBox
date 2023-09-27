#include "Timer.h"

#include <fcntl.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <chrono>
#include "CLog.h"

namespace {
constexpr auto NANOSECS_PER_SECOND = 1000000000ul;
constexpr int MILLS_IN_SECOND = 1000;
constexpr int NANOS_IN_MILL = 1000 * 1000;
}  // namespace

Timer::Timer(Loop *loop, const std::string &name) : Event(name), timerFd_(-1) {
  Timer_ = loop->CreatFdEvent(loop, name);
}

Timer::~Timer() {}

uint64_t Timer::nowSinceEpoch() {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = now.time_since_epoch();
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

uint64_t Timer::fromNow(uint64_t timestamp) {
  auto now = nowSinceEpoch();
  return (timestamp >= now) ? (timestamp - now) : 0;
}

struct timespec Timer::fromNowInTimeSpec(uint64_t timestamp) {
  auto from_now_mills = fromNow(timestamp);
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(from_now_mills / MILLS_IN_SECOND);
  ts.tv_nsec =
      static_cast<int64_t>((from_now_mills % MILLS_IN_SECOND) * NANOS_IN_MILL);
  return ts;
}

bool Timer::init(const std::chrono::nanoseconds first,
                 const std::chrono::nanoseconds repeat) {
  timerFd_ = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerFd_ < 0) {
    return false;
  }
  auto first_nanosec = first.count();
  auto repeat_nanosec = repeat.count();

  timerSpec_.it_value.tv_sec = first_nanosec / NANOSECS_PER_SECOND;
  timerSpec_.it_value.tv_nsec = first_nanosec % NANOSECS_PER_SECOND;
  timerSpec_.it_interval.tv_sec = repeat_nanosec / NANOSECS_PER_SECOND;
  timerSpec_.it_interval.tv_nsec = repeat_nanosec % NANOSECS_PER_SECOND;

  auto mode =
      repeat_nanosec != 0 ? Event::Mode::kPersist : Event::Mode::kOneshot;
  Timer_->init(timerFd_, mode);

  Timer_->setReadCallback(std::bind(&Timer::onTimer, this));
  return true;
}

void Timer::cleanup() { CHECK_CLOSE_RESET_FD(timerFd_); }

bool Timer::start() {
  Timer_->enableReading();
  if (::timerfd_settime(timerFd_, TFD_TIMER_CANCEL_ON_SET, &timerSpec_, NULL) <
      0) {
    return false;
  }
  return true;
}

bool Timer::stop() {
  struct itimerspec ts = {0};
  if (::timerfd_settime(timerFd_, TFD_TIMER_CANCEL_ON_SET, &timerSpec_, NULL) <
      0) {
    return false;
  }
  Timer_->disableReading();
  return true;
}

std::chrono::nanoseconds Timer::remainTime() const {
  std::chrono::nanoseconds remain_time = std::chrono::nanoseconds::zero();
  struct itimerspec ts;
  int ret = ::timerfd_gettime(timerFd_, &ts);
  if (ret == 0) {
    remain_time += std::chrono::seconds(ts.it_value.tv_sec);
    remain_time += std::chrono::nanoseconds(ts.it_value.tv_nsec);
  } else {
    CLOG_ERROR() << name() << ": Get ReMainTime Error";
  }
  return remain_time;
}

void Timer::setTimerCallback(TimerCallback &&cb) {
  timerCallback_ = std::move(cb);
}

void Timer::onTimer() {
  uint64_t exp = 0;
  int len = ::read(timerFd_, &exp, sizeof(exp));
  if (len <= 0) {
    return;
  }
  if (timerCallback_) {
    timerCallback_();
  }
}
