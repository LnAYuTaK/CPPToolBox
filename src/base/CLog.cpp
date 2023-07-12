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

thread_local char _timebuf[64] = {0x00};
CLOG::CLOG() : _ToFile(true), _ToTerminal(true), _MxLogBufferSize(256) {}
/***********************************************************/
CLOG::~CLOG() {}
/***********************************************************/
//[2023.06.28-01:31:21]
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
//[2023.06.28-01:31:21]
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
