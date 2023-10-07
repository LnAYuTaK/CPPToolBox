/**
 * @file IODevice.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  外设设备基类
 * @version 0.1
 * @date 2023-07-02
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <iostream>
#include <string>

namespace gwcpp
{
class IODevice {
 public:
  enum State {
    // 仅完成了构造，还没有init
    kNone,
    //已init 还没有start 或者正处于stop状态
    kInited,
    //正在运行状态
    kRunning
  };
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
  inline State state() const { return state_; }
  void setState(State state) { state_ = state; }
 private:
  State state_ = kNone;
};

  
} // namespace gwcpp

