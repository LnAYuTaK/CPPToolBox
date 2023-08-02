/**
 * @file CLog.h
 * @author 刘宽 (807874484@qq.com)
 * @brief  日志模块
 * 注意文件是同步写入 大量IO操作可能导致性能问题
 * 考虑实现一个异步文件读写
 * @version 0.1
 * @date 2023-06-13
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once
#include <cstdio>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include "LogStream.h"
#include "MacroDef.h"

class CLOG {
  DECLARE_SINGLETON(CLOG)
 public:
  enum CLOG_LEVEL {
    CLOG_LEVEL_DEBUG,
    CLOG_LEVEL_INFO,
    CLOG_LEVEL_WARN,
    CLOG_LEVEL_ERROR,
  };

  ~CLOG();

  static unsigned long long GetCurrentThreadId();

  static std::string GetCurrentData();

  static std::string GetCurrentDateTime();

  static char* GetCurrentTime();
  // TUDO
  void CLOGPrint(CLOG_LEVEL nLevel, const char* pcFunc, const int& line,
                 const char* fmt, ...);
  // To File
  inline bool isToFile() { return _ToFile; }

  inline void setToFile(bool select) { this->_ToFile = select; }
  //
  class LogMsg {
   public:
    // DEBUG INFO
    LogMsg(CLOG_LEVEL nLevel, const char* pcFunc, const int& line);
    // WARN ERROR
    LogMsg(CLOG_LEVEL nLevel, const char* pcFunc, const char* file,
           const int& line);
    ~LogMsg();
    LogStream& stream() { return _stream; }

   private:
    std::mutex _mtx;
    int nLevel_;
    LogStream _stream;
  };

 private:
  // Tudo
  void writeLogLevel(char* buffer, CLOG_LEVEL nLevel);
  //-------------------------------
  std::string _LogFileName;  //日志文件名
  bool _ToFile;              //是否允许写入文件
  bool _ToTerminal;          //是否允许控制台
  std::mutex _WriteMtx;      //写锁
  int _MxLogBufferSize;      //最大输出日志长度
};

// C Style LOG
#define CLOG_INFO_FMT(fmt, args...)                                            \
  CLOG::Instance()->CLOGPrint(CLOG::CLOG_LEVEL::CLOG_LEVEL_INFO, __FUNCTION__, \
                              __LINE__, fmt, ##args)
#define CLOG_WARN_FMT(fmt, args...)                                 \
  CLOG::Instance()->CLOGPrint(CLOG::CLOG_LEVEL::CLOG_LEVEL_WARNING, \
                              __FUNCTION__, __LINE__, fmt, ##args)
#define CLOG_ERROR_FMT(fmt, args...)                              \
  CLOG::Instance()->CLOGPrint(CLOG::CLOG_LEVEL::CLOG_LEVEL_ERROR, \
                              __FUNCTION__, __LINE__, fmt, ##args)
// CPP Style LOG
#define CLOG_DEBUG()                                                       \
  CLOG::LogMsg(CLOG::CLOG_LEVEL::CLOG_LEVEL_DEBUG, __FUNCTION__, __LINE__) \
      .stream()
#define CLOG_INFO()                                                       \
  CLOG::LogMsg(CLOG::CLOG_LEVEL::CLOG_LEVEL_INFO, __FUNCTION__, __LINE__) \
      .stream()
#define CLOG_WARN()                                                       \
  CLOG::LogMsg(CLOG::CLOG_LEVEL::CLOG_LEVEL_WARN, __FUNCTION__, __LINE__) \
      .stream()
#define CLOG_ERROR()                                                       \
  CLOG::LogMsg(CLOG::CLOG_LEVEL::CLOG_LEVEL_ERROR, __FUNCTION__, __LINE__) \
      .stream()
