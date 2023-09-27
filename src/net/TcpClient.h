/**
 * @file TcpClient.cpp
 * @author 刘宽 (807874484@qq.com)
 * @brief TCP客户端组件
 * @version 0.1
 * @date 2023-08-04
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <memory>
#include "Address.h"

#include "Loop.h"
#include "Connector.h"

class TcpClient {
 public:

  using ConnectCallback = std::function<void(const TcpConnection::ptr &)>;
  using ReceiveCallback = std::function<void(const TcpConnection::ptr &, void *, ssize_t)>;
  using CloseCallback   =  std::function<void(const TcpConnection::ptr &)>;

  explicit TcpClient(Loop *loop,const Address::ptr address);
  virtual ~TcpClient();

  //!< 状态
  enum State {
    None,        //!< 未初始化
    Inited,      //!< 已初始化
    Connecting,  //!< 连接中
    Connected    //!< 连接成功
  };
  /**
   * @brief 启动客户端
   *
   * @return true
   * @return false
   */
  bool start();
  /**
   * @brief 客户端停止
   *
   */
  void stop();
  /**
   * @brief 半关闭状态 //未完成TUDO
   *
   */
  void shutdown();
  /**
   * @brief 返回当前状态
   *
   * @return State
   */
  State state() const;
  /**
   * @brief 设置创建连接成功回调
   *
   * @param new_conn
   */
  void setConnectCallback(const ConnectCallback &&cb);
  /**
   * @brief 设置接收数据回调
   *
   * @param cb
   */
  void setReceiveCallback(const ReceiveCallback &&cb);
  /**
   * @brief 断线回调
   *
   * @param cb
   */
  void setCloseCallback(const CloseCallback &&cb);
  /**
   * @brief 设置自动重连
   * 
   * @param enable 
   */
  void setAutoReconnect(bool enable);

 private:
  void newConnection(Socket::ptr &sock_ptr);
  void OnTcpDisConnected();
  void cleanup();
 private:
  Loop *loop_;
  TcpClient::State state_;
  //连接器
  Connector::ptr connector_;
  //当前连接//
  TcpConnection::ptr connection_;
  //回调处理
  ConnectCallback connected_cb;
  ReceiveCallback receive_cb;
  CloseCallback   close_cb;
  bool recon_enable_;
};
