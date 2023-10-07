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
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <memory>

#include "Address.h"
#include "gwcpp/event/Fd.h"
#include "gwcpp/base/CLog.h"
#include "gwcpp/base/MacroDef.h"
#include "gwcpp/base/Utils.h"

namespace gwcpp
{

class Socket {
 public:
  using  ptr     =  std::unique_ptr<Socket>;
  enum ErroSocketOpt {
    UNKNOWN = 0,  // Unknown
    ACCEPT = 1,   // Acccept
    CONNECT = 2,  // Connect
    SEND = 3,     // Send
    RECEIVE = 4,  // Receive
    CLOSE = 5,    // Close
  };
  Socket(int protocol ,int type);
  ~Socket();
  /**
   * @brief 初始化Socket
   * 
   * @return true 
   * @return false 
   */
  bool newSocket();
  /**
   * @brief 创建TCP Socket(满足地址类型)
   * @param[in] address 地址
   */
  /**
   * @brief 创建IPv4的UDP Socket
   */
  static Socket::ptr CreateUdpSocket();
  /**
   * @brief 创建IPv4的TCP Socket
   */
  static Socket::ptr CreateTcpSocket();
  // /**
  //  * @brief 创建IPv6的UDP Socket
  //  * 
  //  * @return Socket::ptr 
  //  */
  // static Socket::ptr CreateV6UdpSocket();
  // /**
  //  * @brief 创建IPv6的TCP Socket
  //  *    
  //  * @return Socket::ptr 
  //  */
  // static Socket::ptr CreateV6TcpSocket();
 public:
  /**
   * @brief 连接
   *
   * @param addr
   * @return int
   */
  int connect(const Address::ptr addr);
  /**
   * @brief 绑定
   *
   * @param server_address
   * @return int
   */
  bool bind(const Address::ptr addr);
  /**
   * @brief 监听
   *
   * @return int
   */
  bool listen();
  /**
   * @brief 
   * 
   * @param buffer 待发送数据的内存
   * @param length 待发送数据的长度
   * @param flags 标志字
     * @return
     *      @retval >0 发送成功对应大小的数据
     *      @retval =0 socket被关闭
     *      @retval <0 socket出错
     */
  ssize_t send(const void* buffer, size_t length, int flags = 0);
  /**
   * @brief 实例化对端socket
   * 
   * @return 成功返回 Socket::ptr  失败返回 nullptr
   */
  Socket::ptr accept();
  /**
   * @brief 判断当前Socket是否无效
   *
   */
  bool isInvalid();
  /**
   * @brief 是否连接
   * 
   * @return true 
   * @return false 
   */
  bool isConnected() const;
  /**
   * @brief 获取Socket的文件描述符
   *
   * @return int
   */
  int getFd();
  /**
   * @brief 关闭socket
   *
   */
  void close();
  /**
   * @brief 关闭读写流
   *
   */
  void shutDownWrite();
  /**
   * @brief 获取本地地址
   * 
   * @return Address::ptr
   */
  Address::ptr  getLocalAddress();
  /**
   * @brief 获取对端地址
   * 
   * @return Address::ptr 
   */
  Address::ptr getPeerAddress();
  /**
   * @brief 
   * 
   * @return true 
   * @return false 
   */
  bool checkConnected();
  /**
   * @brief 判断是不是同一链接
   * 
   * @param sock 
   * @return true 
   * @return false 
   */
  bool isSelfConnect();
 public:  // socket相关的设置
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
   * @brief 设置保活机制
   * 
   * @param enale 是否开启保活机制
   * @param idel  探测间隔时间
   * @param interval 探测时发包的时间间隔
   * @param count  探测尝试次数(如果第1次探测包就收到响应了,则后2次的不再发)
   * @return true 
   * @return false 
   */
  bool setKeepAlive(bool enale);
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
   
  int getSocketError(); 
  
  bool isConnected_;
private:
    bool init(int sock);
private:
  Fd fd_;
  /// 类型
  int type_;
  // 协议
  int protocol_;
  /// 本地地址
  Address::ptr localAddress_;
  /// 远端地址
  Address::ptr remoteAddress_;
};
} // namespace gwcpp
