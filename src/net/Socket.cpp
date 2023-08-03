#include "Socket.h"

#include <sys/socket.h>
#include <memory>

#include "CLog.h"
#include "MacroDef.h"

#include "NetAddress.h"

static constexpr int BACK_LOG = 128;


Socket::Socket(int family, int type, int protocol)
{ 
    int socket_fd = ::socket(family,type,protocol);
    if(LIKELY(socket_fd != -1)) {
      int val = 1;
      //设置地址重用
      setSocketOpt(SOL_SOCKET, SO_REUSEADDR, val);
      //设置禁止Nagle算法
      // if(type == Type::TCP) {
      //   setSocketOpt(IPPROTO_TCP,TCP_NODELAY, val);
      // }
    }
}

Socket::ptr Socket::CreateUdpSocket() {
  return std::make_shared<Socket>(Socket::IPv4, Socket::UDP);
}

Socket::ptr Socket::CreateTcpSocket() {
  return std::make_shared<Socket>(Socket::IPv4, Socket::TCP);
}

int Socket::connect(NetAddress &adress) {
  int ret = ::connect(fd_.get(), adress.addr(), *(adress.addrLen()));
  if (ret < 0) {
    CLOG_ERROR() << "Connect Error";
  }
  return ret;
}

int Socket::bind(NetAddress &adress) {
  int ret = ::bind(fd_.get(), adress.addr(), *(adress.addrLen()));
  if (ret < 0) {
    CLOG_ERROR() << "Bind Error";
  }
  return ret;
}

int Socket::listen() {
  int ret = ::listen(fd_.get(), BACK_LOG);
  if (ret < 0) {
    CLOG_ERROR() << "Listen Error";
  }
  return ret;
}

int Socket::accept(NetAddress &adress) {
  int ret = ::accept(fd_.get(), adress.addr(), adress.addrLen());
  if (ret < 0) {
    CLOG_ERROR() << "Accept Error";
  }
  return ret;
}

bool Socket::getSocketOpt(int level, int optname, void *optval,
                          socklen_t *optlen) {
  if (::getsockopt(fd_.get(), level, optname, optval, optlen) != 0) {
    CLOG_ERROR() << "Get Socket Opt Error";
    return false;
  }
  return true;
}

bool Socket::setSocketOpt(int level, int optname, int optval) {
  if (::setsockopt(fd_.get(), level, optname, &optval, sizeof(optval)) != 0) {
    return false;
  }
  return true;
}

bool Socket::setSocketOpt(int level, int optname, const void *optval,
                          socklen_t optlen) {
  if (::setsockopt(fd_.get(), level, optname, optval, optlen) != 0) {
    return false;
  }
  return true;
}

bool Socket::setReuseAddress(bool enable) {
  return setSocketOpt(SOL_SOCKET, SO_REUSEADDR, enable);
}

bool Socket::setKeepalive(bool enable) {
  return setSocketOpt(SOL_SOCKET, SO_KEEPALIVE, enable);
}

bool Socket::setRecvBufferSize(int size) {
  return setSocketOpt(SOL_SOCKET, SO_RCVBUF, size);
}

bool Socket::setSendBufferSize(int size) {
  return setSocketOpt(SOL_SOCKET, SO_SNDBUF, size);
}
