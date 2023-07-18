/**
 * @file BufferPtr.h
 * @brief 外设设备基类
 * @version 0.1
 * @date 2023-06-26
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <iostream>
#include <string>

class SerialDevice;
class ADCDevice;
class EpollLoop;
class IODevice {
 public:
  IODevice() {}
  virtual ~IODevice() {}
  //初始化
  virtual bool init() { return true; }
  //开启
  virtual bool start() = 0;
  //停止
  virtual void stop() = 0;
  //关闭
  virtual void close() = 0;
  //清理
  virtual void cleanup() = 0;
  // name
  std::string name() const { return name_; }
  std::string name_;
  static SerialDevice* creatSerialDevice(EpollLoop* loop,
                                         const std::string& name);
};
