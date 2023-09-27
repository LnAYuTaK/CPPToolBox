/**
 * @file TcpConnection.h
 * @author 刘宽 (807874484@qq.com)
 * @brief TCP连接对象
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <memory>
#include "ByteBuf.h"
#include "Socket.h"
#include "EpollFdEvent.h"
class Loop;
class TcpClient;
// Using
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
  friend class Connector;
 public:
  enum State { 
    Disconnected, 
    Connecting, 
    Connected, 
    Disconnecting 
  };

  typedef std::shared_ptr<TcpConnection> ptr;
  typedef std::weak_ptr<TcpConnection>  weak_ptr;

  using ConnectCallback = std::function<void(const TcpConnection::ptr &)>;
  using ReceiveCallback = std::function<void(const TcpConnection::ptr &, void *, ssize_t)>;
  using CloseCallback   = std::function<void(const TcpConnection::ptr &)>;

  explicit TcpConnection(Loop *loop, Socket::ptr &socket);
  virtual ~TcpConnection();
  /**
   * @brief 发送信息
   * 
   * @param message 
   * @param len 
   */
  void send(const void *message, int len);
  void send(ByteBuf &buf);
  //主动断开
  void disconnect();

  State &state() { return state_; }
  /**
   * @brief 设置连接成功回调函数
   * 
   * @param cb 
   */
  void setConnectionCallback(const ConnectCallback &cb);
  /**
   * @brief 设置接受信息回调
   * 
   * @param cb 
   */
  void setReceiveCallback(const ReceiveCallback &cb);
  /**
   * @brief 设置关闭回调函数
   * 
   * @param cb 
   */
  void setCloseCallback(const CloseCallback &cb);

  void connectFinished();
 private:

  void handleWrite();
  void handleClose();
  void handleRead();
  void handleError();

 private:
  Loop *loop_;
  State state_;
  Socket::ptr socket_;
  EpollFdEvent::ptr event_;

  std::unique_ptr<ByteBuf> read_buffer_;
  std::unique_ptr<ByteBuf> write_buffer_;

  ConnectCallback connected_cb;
  ReceiveCallback receive_cb_;
  CloseCallback close_cb_;
};
