#include "Socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h> 
#include <netinet/in.h>
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
      //小数据量还是取消Nagle好点
      if(type == Type::TCP) {
        setSocketOpt(IPPROTO_TCP,TCP_NODELAY, val);
      }
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

bool Socket::getSocketOpt(int level, int opt_name, void *opt_val,
                          socklen_t *opt_len) {
  if (::getsockopt(fd_.get(), level, opt_name, opt_val, opt_len) != 0) {
    CLOG_ERROR() << "Get Socket Opt Error";
    return false;
  }
  return true;
}

bool Socket::setSocketOpt(int level, int opt_name, int opt_val) {
  if (::setsockopt(fd_.get(), level, opt_name, &opt_val, sizeof(opt_val)) != 0) {
    return false;
  }
  return true;
}

bool Socket::setSocketOpt(int level, int opt_name, const void *opt_val,
                          socklen_t opt_len) {
  if (::setsockopt(fd_.get(), level, opt_name, opt_val, opt_len) != 0) {
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
