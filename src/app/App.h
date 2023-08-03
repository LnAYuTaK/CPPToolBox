/**
 * @file App.h
 * @author 刘宽 (807874484@qq.com)
 * @brief 应用程序APP(单例)
 * @version 0.1
 * @date 2023-06-26
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once
// std
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "ByteBuf.h"
// user
#include "CLog.h"
#include "FileHelper.h"
#include "IOControl.h"
#include "MacroDef.h"
#include "ThreadPool.h"
// app
#include "ADCDevice.h"
#include "Database.h"
#include "EpollFdEvent.h"
#include "FdEvent.h"
#include "Loop.h"
#include "SerialDevice.h"
#include "TimerEvent.h"

class Application {
  //单例
  DECLARE_SINGLETON(Application)
 public:
  ~Application();
  //资源初始化
  virtual void init();
  //开启系统任务
  virtual void start();
  // Event Loop;
  void exec();
  // Main Loop
  EpollLoop* loop() { return this->loop_; }
  // SerialDevice* uart1;
 private:
  EpollLoop* loop_ = nullptr;
};

//全局单例接口
Application* app();
