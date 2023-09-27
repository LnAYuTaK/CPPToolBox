/**
 * @file Acceptor.h
 * @author 刘宽 (807874484@qq.com)
 * @brief  用于接收新的TCP连接，供TcpServer使用，生命期由TcpServer控制
 * @version 0.1
 * @date 2023-09-08
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

#include "ByteBuf.h"
#include "MacroDef.h"

#include "Address.h"
#include "Socket.h"
#include "TcpConnection.h"

class Loop;
class EpollFdEvent;
class Acceptor {
   DISALLOW_COPY_AND_ASSIGN(Acceptor)
 public:
  using TCPConnPtr = std::shared_ptr<TcpConnection>;
  //新的连接回调
  using NewConnectCallback = std::function<void(Socket::ptr &)>;
  enum class State {
    Inited,          //初始状态
    Started,         //启动
    Stoped           //暂停状态
  };
 public:
  Acceptor(Loop *loop,const Address::ptr address);
  virtual ~Acceptor();

  /**
   * @brief 启动服务器
   * 
   * @param async_connect 
   * @return true 
   * @return false 
   */
  bool start(bool async_connect);
  /**
   * @brief 服务器暂停
   * 
   */
  void stop();
  /**
   * @brief 当前连接器状态
   * 
   * @return State 
   */
  State state() const;
  /**
   * @brief 设置到新的客户端连接回调
   * 
   * @param cb 
   */
  void setNewConnectionCallback(NewConnectCallback &&cb);
 private:
  void handleRead();
  void cleanup();
 private:
  Loop *loop_;
  //当前状态
  State state_;
  //
  Address::ptr addr_;
  //
  EpollFdEvent::ptr event_;
  // TCP Socket
  Socket::ptr sock_ptr;
  //
  std::mutex conn_mtx;
};
