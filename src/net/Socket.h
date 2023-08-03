/**
 * @file Socket.h
 * @author 刘宽 (807874484@qq.com)
 * @brief Socket
 * @version 0.1
 * @date 2023-07-24
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <errno.h>
#include <sys/socket.h>
#include <cstring>
#include  <memory>
#include "Fd.h"
class NetAddress;
class Socket{
 public:
  using ptr =  std::shared_ptr<Socket>;
  using weak_ptr =  std::weak_ptr<Socket>;

  enum  Type {
        /// TCP类型
        TCP = SOCK_STREAM,
        /// UDP类型
        UDP = SOCK_DGRAM
  };

  enum  Family {
        /// IPv4 socket
        IPv4 = AF_INET,
        /// IPv6 socket
        IPv6 = AF_INET6,
        /// Unix socket
        UNIX = AF_UNIX,
  };

 public:
  Socket(int family, int type, int protocol = 0);
  
  static Socket::ptr CreateUdpSocket();
  static Socket::ptr CreateTcpSocket();

 public:
  int connect(NetAddress &adress);
  int bind(NetAddress &server_address);
  int listen();
  int accept(NetAddress &client_address);

 public:  //! socket相关的设置
  bool getSocketOpt(int level, int optname, void *optval, socklen_t *optlen);
  bool setSocketOpt(int level, int optname, int optval);
  bool setSocketOpt(int level, int optname, const void *optval,
                    socklen_t optlen);
  /**
   * @brief 设置可重用地址
   *
   * @param enable
   * @return true
   * @return false
   */
  bool setReuseAddress(bool enable);
  /**
   * @brief 设置是否开启保活
   *
   * @param enable
   * @return true
   * @return false
   */
  bool setKeepalive(bool enable);
  /**
   * @brief 设置接收缓冲大小
   *
   * @param size
   * @return true
   * @return false
   */
  bool setRecvBufferSize(int size);
  /**
   * @brief 设置发送缓冲大小
   *
   * @param size
   * @return true
   * @return false
   */
  bool setSendBufferSize(int size);

  private:
    Fd fd_;
};
