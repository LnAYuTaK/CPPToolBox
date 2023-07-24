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

#include "Fd.h"
#include "MacroDef.h"

class NetAddress;
enum class Protocol;
class SocketFd : public Fd {
  public:
    SocketFd(const Fd &fd);
  public:
    static SocketFd CreateSocket(int domain, int type, int protocal);
    static SocketFd CreateUdpSocket();
    static SocketFd CreateTcpSocket();

  public: //! socket相关的操作
    int connect(const struct sockaddr *addr, socklen_t addrlen);

    int bind(const struct sockaddr *addr, socklen_t addrlen);

    int listen(int backlog);

    int accept(struct sockaddr *addr, socklen_t *addrlen);

    ssize_t send(const void* data_ptr, size_t data_size, int flag);
    ssize_t recv(void *data_ptr, size_t data_size, int flag);

    ssize_t sendTo(const void* data_ptr, size_t data_size, int flag,
                   const sockaddr *dest_addr, socklen_t addrlen);
    ssize_t recvFrom(void* data_ptr, size_t data_size, int flag,
                     sockaddr *dest_addr, socklen_t *addrlen);

    int shutdown(int howto);

  public: //! socket相关的设置
    bool getSocketOpt(int level, int optname, void *optval, socklen_t *optlen);
    bool setSocketOpt(int level, int optname, int optval);
    bool setSocketOpt(int level, int optname, const void *optval, socklen_t optlen);

    bool setReuseAddress(bool enable);  //! 设置可重用地址
    bool setBroadcast(bool enable);     //! 设置是否允许广播
    bool setKeepalive(bool enable);     //! 设置是否开启保活

    bool setRecvBufferSize(int size);   //! 设置接收缓冲大小
    bool setSendBufferSize(int size);   //! 设置发送缓冲大小
};
