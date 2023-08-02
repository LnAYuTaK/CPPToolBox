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
#include "CLog.h"
#include "Fd.h"
#include "MacroDef.h"

class NetAddress;
enum class Protocol;
class Socket : public Fd {
 public:
  Socket() noexcept = default;
  Socket(const Fd &fd);

 public:
  static Socket createSocket(int domain, int type, int protocal);
  static Socket createUdpSocket();
  static Socket createTcpSocket();

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
};
