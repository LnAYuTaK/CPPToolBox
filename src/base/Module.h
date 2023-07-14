/**
 * @file Module.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once 

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

class Module {
  public:

  Module(const std::string &name)
          :name_(name)
  {}
                      
  virtual ~Module(){}
  //! 状态
  enum class State {
        kNone,  
        kInited,   
        kRunning   
  };
 inline std::string name()  { return name_; }
 inline State state() const { return state_; }

  virtual bool init()= 0;
  virtual bool start()= 0;
  virtual void stop() = 0;
  virtual void cleanup() = 0;

  virtual void onFillDefaultConfig(){}
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

