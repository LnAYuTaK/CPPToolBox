

/**
 * @file TcpConnection.cpp
 * @author 刘宽 (807874484@qq.com)
 * @brief Connector
 * Tcp 客户端的连接器 负责连接服务器操作
 *
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <mutex>

#include "MacroDef.h"
#include "EpollFdEvent.h"
#include "Address.h"
#include "Socket.h"
#include "TcpConnection.h"

class Loop;
class Connector {

  DISALLOW_COPY_AND_ASSIGN(Connector)
public:
  using ptr = std::unique_ptr<Connector>;
  using NewConnectCallback = std::function<void(Socket::ptr &)>;
  //状态
  enum class State {
      Disconnected, 
      Connecting, 
      Connected
  };
  //回调函数定义
  explicit Connector(Loop *loop,const Address::ptr addr);
  virtual ~Connector();
/**
 * @brief 客户端连接器
 * 
 * @param async_connect 
 * @return true 
 * @return false 
 */
  bool start(bool async_connect = false);
  /**
   * @brie 
   * 
   */
  void reStart();
  /**
   * @brief 主动停止连接器
   *
   */
  void stop();
  /**
   * @brief  当前状态
   *
   * @return State
   */
  State state() const;

  /**
   * @brief 
   * 
   */
  void resetEvent();
  /**
   * @brief 设置自动重连功能
   * 
   * @param enable 
   */
  void setAutoReconnect(bool enable);
  //设置连接成功回调
  void setNewConnectionCallback(const NewConnectCallback &&cb);

 private:
  void handleWrite();
  int  removeAndResetChannel();
  void cleanup();
 private:
  Loop *loop_;
  //当前状态
  State state_;
  //服务器目标地址
  Address::ptr addr_;
  //回调
  NewConnectCallback connect_cb_;
  //连接事件
  EpollFdEvent::ptr event_;
  // 这个socket只做为临时变量不要使用
  Socket::ptr sock_ptr;
  //重连次数
  int try_times_;
  //延时时间
  int delay_sec_;
  //是否是一直重连
  bool conn_forever;
  bool connect_; // atomic
  //
  std::mutex conn_mtx;
};
