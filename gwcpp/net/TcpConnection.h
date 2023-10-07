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
#include "Socket.h"

#include "gwcpp/base/ByteBuf.h"
#include "gwcpp/event/EpollFdEvent.h"
#include "gwcpp/base/CLog.h"

namespace gwcpp
{

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

  using ptr       =  std::shared_ptr<TcpConnection> ;
  using weak_ptr  =  std::weak_ptr<TcpConnection> ;

  using ConnectCallback = std::function<void(const TcpConnection::ptr &)>;
  using ReceiveCallback = std::function<void(const TcpConnection::ptr &, void *, ssize_t)>;
  using CloseCallback   = std::function<void(const TcpConnection::ptr &)>;

  explicit TcpConnection(Loop *loop, Socket::ptr &socket,const std::string& name);
  virtual ~TcpConnection();
 
  /**
   * @brief name
   * 
   * @return std::string 
   */
  std::string name();
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
  std::string name_;
  EpollFdEvent::ptr event_;

  std::unique_ptr<ByteBuf> read_buffer_;
  std::unique_ptr<ByteBuf> write_buffer_;

  ConnectCallback connected_cb;
  ReceiveCallback receive_cb_;
  CloseCallback close_cb_;
};

  
} // namespace gwcpp
