/**
 * @file BufferPtr.h
 * @brief 外设设备基类
 * @version 0.1
 * @date 2023-06-26
 * @copyright Copyright (c) 2023
 */

#pragma once

class IODevice {
 public:
  enum DriverType { SerialType, ADCDeviceType, SpiType, GPIOType};
  IODevice(/* args */) { ; }
  virtual DriverType type() const = 0;
  
  //Fill Json Para
  virtual bool init()
  {
    return true;
  }
  //
  virtual bool start() = 0;
  virtual void stop() = 0;
  virtual void close()  = 0;
  virtual void cleanup() = 0;

  virtual ~IODevice() { ; }
};
