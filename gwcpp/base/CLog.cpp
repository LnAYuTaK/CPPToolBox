#include "CLog.h"

#include <memory.h>
#include <stdarg.h>
#include <sys/time.h>
#include <cstdarg>
#include <ctime>
#include <sstream>
#include <thread>

#include <chrono>
#include <iomanip>
using namespace gwcpp;

static constexpr const char *LogLevelName[4] = {" DEBUG", " INFO ", " WARN ",
                                                " ERROR"};
// Not Used
static constexpr const int LogLevelColor[4] = {34, 32, 33, 31};
thread_local char _timebuf[64] = {0x00};
CLOG::CLOG() : _ToFile(true), _ToTerminal(true), _MxLogBufferSize(256) {}
/***********************************************************/
CLOG::~CLOG() {}
/***********************************************************/
CLOG::LogMsg::LogMsg(CLOG_LEVEL nLevel, const char *pcFunc, const int &line) {
  std::lock_guard<std::mutex> lock(_mtx);
  nLevel_ = (int)nLevel;
  _stream.zeroBuffer();
  _stream << "[" << CLOG::GetCurrentDateTime() << "]"
          << "[" << LogLevelName[(int)nLevel] << "]"
          << "[" << pcFunc << "]"
          << "[" << line << "]";
}
/***********************************************************/
CLOG::LogMsg::LogMsg(CLOG_LEVEL nLevel, const char *pcFunc,
                     const char *file_name, const int &line) {
  std::lock_guard<std::mutex> lock(_mtx);
  nLevel_ = (int)nLevel;
  _stream.zeroBuffer();
  _stream << "[" << CLOG::GetCurrentDateTime() << "]"
          << "[" << LogLevelName[nLevel] << "]"
          << "[" << pcFunc << "]"
          << "[" << CLOG::baseFileName(file_name) << "]"
          << "[" << line << "]";
}
/***********************************************************/
CLOG::LogMsg::~LogMsg() {
  std::lock_guard<std::mutex> lock(_mtx);
  if (CLOG::Instance()->isToFile()) {
    std::fstream f;
    std::string s = CLOG::GetCurrentData() + ".log";
    f.open(s, std::fstream::out | std::fstream::app);
    f << _stream.buffer().data() << std::endl;
    if (f.is_open()) {
      f.flush();
      f.close();
    }
  }
  fprintf(stdout, "\033[%dm%s\e[0m\n", LogLevelColor[nLevel_],
          _stream.buffer().data());
  fflush(stdout);
}
/***********************************************************/
std::string CLOG::GetCurrentDateTime() {
  // 获取当前系统时间
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  // 将时间转换为本地时间
  std::tm *localTime = std::localtime(&time);
  // 格式化输出
  std::ostringstream oss;
  oss << std::put_time(localTime, "%Y.%m.%d-%H:%M:%S");
  return oss.str();
}
/***********************************************************/
char *CLOG::GetCurrentTime() {
  time_t t = time(nullptr);
  struct tm *now = localtime(&t);
  strftime(_timebuf, sizeof(_timebuf), "%Y.%m.%d-%H:%M:%S", now);
  now = nullptr;
  return _timebuf;
}
/***********************************************************/
unsigned long long CLOG::GetCurrentThreadId() {
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  std::string stid = oss.str();
  unsigned long long tid = stoull(stid);
  return tid;
}
/***********************************************************/
std::string CLOG::GetCurrentData() {
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  char buffer[9];
  Utils::memZero(buffer, sizeof(buffer));
  std::strftime(buffer, sizeof(buffer), "%Y%m%d", &tm);
  return std::string(buffer);
}
/***********************************************************/
void CLOG::writeLogLevel(char *buffer, CLOG_LEVEL nLevel) {
  switch (nLevel) {
    case CLOG_LEVEL::CLOG_LEVEL_INFO:
      sprintf(buffer, "%s", " INFO ");
      break;
    case CLOG_LEVEL::CLOG_LEVEL_WARN:
      sprintf(buffer, "%s", " WARN ");
      break;
    case CLOG_LEVEL::CLOG_LEVEL_ERROR:
      sprintf(buffer, "%s", " ERROR");
      break;
    default:
      sprintf(buffer, "%s", "UNKNOW");
      break;
  }
}
/***********************************************************/
const char *CLOG::baseFileName(const char *full_path) {
  const char *p_last = full_path;
  if (p_last != nullptr) {
    for (const char *p = full_path; *p; ++p) {
      if (*p == '/') p_last = p + 1;
    }
  }
  return p_last;
}
/***********************************************************/
void CLOG::CLOGPrint(CLOG_LEVEL nLevel, const char *pcFunc, const int &line,
                     const char *fmt, ...) {
  std::lock_guard<std::mutex> lock(_WriteMtx);
  char levelBuffer[32];
  char buffer[_MxLogBufferSize];
  memset(buffer, 0, _MxLogBufferSize);
  memset(levelBuffer, 0, 32);
  writeLogLevel(levelBuffer, nLevel);
  int n = sprintf(buffer,
                  "[%s]"
                  "[%s]"
                  "[%s][%d]",
                  GetCurrentTime(), levelBuffer, pcFunc, line);
  va_list vap;
  va_start(vap, fmt);
  vsnprintf(buffer + n, _MxLogBufferSize - n, fmt, vap);
  va_end(vap);
  if (_ToFile) {
    std::fstream f;
    std::string s = GetCurrentData() + ".log";
    f.open(s, std::fstream::out | std::fstream::app);
    if (f.is_open()) {
      f << buffer << '\n';
      f.flush();
      f.close();
    }
    fprintf(stdout, "%s\n", buffer);
    fflush(stdout);
  }
} 


