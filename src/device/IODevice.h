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
class UartDevice;
class ADCDevice;
class EpollLoop;

class IODevice {
 public:
  enum DriverType { SerialType, ADCDeviceType, SpiType, GPIOType};
  IODevice() {}
  virtual~IODevice(){}
  virtual DriverType type() const = 0;
  //初始化
  virtual bool init(){return true;}
  //开启
  virtual bool start() = 0;
  //停止
  virtual void stop() = 0;
   //关闭
  virtual void close()  = 0;
   //清理
  virtual void cleanup() = 0;

  UartDevice *  creatUartDevice(EpollLoop * loop , std::string & name);
};
