/**
 * @file App.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  应用程序Application (全局单例)
 * @version 0.1
 * @date 2023-06-26
 * @copyright Copyright (c) 2023
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
#include "EpollFdEvent.h"
#include "TimerEvent.h"
#include "FdEvent.h"
#include "Loop.h"
#include "Database.h"
#include "TCPClient.h"
#include "TCPServer.h"
#include "ADCDevice.h"
#include "SerialDevice.h"

//TEST

#define FD_EVENT_TEST
#define DB_FILE "test.db"
// #define MQTT_TEST
// #define NETWORK_TEST
//#define  DATABASE_TEST
class Application {
  //单例
  DECLARE_SINGLETON(Application)
 public:
  ~Application();
  //资源初始化
  void init();
  //开启系统任务
  void start();
  // Event Loop;
  void exec() {
    if (loop_) {
      loop_->runLoop();
    }
  }
  // Main Loop
  EpollLoop* loop() { return this->loop_; }
  // SerialDevice* uart1;
 private:
  EpollLoop* loop_ = nullptr;


};

//全局单例接口
Application* app();
