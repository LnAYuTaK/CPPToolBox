/**
 * @file Module.h
 * @author 刘宽 (807874484@qq.com)
 * @brief   基础单元模块
 * @version 0.1
 * @date 2023-07-29
 * 
 *  Copyright (c) 2023 国网中电研发部
 * 
 */
#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class Module {
 public:
  Module(const std::string &name) : name_(name) {}

  virtual ~Module() {}
  //! 状态
  enum class State { kNone, kInited, kRunning };
  inline std::string name() { return name_; }
  inline State state() const { return state_; }

  virtual bool init(){return true;}
  virtual bool start() = 0;
  virtual void stop() = 0;
  virtual void close()  = 0;
  virtual void cleanup() = 0;

  virtual void onFillDefaultConfig() {}
  //! 初始化本模块的操作，可重写
  virtual bool onInit() { return true; }
  //! 启动本模块的操作，可重写
  virtual bool onStart() { return true; }
  //! 停止本模块的操作，可重写，对应onStart()的逆操作
  virtual void onStop() {}
  //! 清理本模块的操作，可重写，对应onInit()的逆操作
  virtual void onCleanup() {}

 private:
  std::string name_;
  State state_ = State::kNone;
};
