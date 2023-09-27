#include "Socket.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <memory>

#include "CLog.h"
#include "MacroDef.h"
#include "Utils.h"
//Listen队列长度
static constexpr int BACK_LOG = 128;
//保活时间
static constexpr int KEEP_IDLE  = 60;
//保活包间隔时间
static constexpr int KEEP_INTEL = 10;
//保活包发送个数
static constexpr int KEEP_CNT   = 2;

Socket::Socket(int protocol,int type)
  :isConnected_(false)
  ,type_(type)
  ,protocol_(protocol){
}

Socket::~Socket(){
  CLOG_INFO() << "Socket SLOSE";
  fd_.close();
}

bool Socket::newSocket() {
  int socket_fd = ::socket(protocol_, type_, 0);
  fd_ = Fd(socket_fd);
  if (socket_fd != -1) {
    int val = 1;
    //设置地址重用
    if (!setSocketOpt(SOL_SOCKET, SO_REUSEADDR, val)) {
      return false;
    }
    //小数据量还是取消Nagle好点
    if (type_ == Address::Type::TCP) {
      if (!setSocketOpt(IPPROTO_TCP, TCP_NODELAY, val)) {
        return false;
      }
    }
  } else {
    return false;
  }
  return true;
}

bool Socket::init(int sock) {
    fd_ = Fd(sock);
    if(!fd_.isNull()){
      isConnected_ = true;
        int val = 1;
        //设置地址重用
        if (!setSocketOpt(SOL_SOCKET, SO_REUSEADDR, val)) {
          CLOG_ERROR() << "Socket Set REUSEADDR  Error";
        }
        if (type_ == Address::Type::TCP) {
          if (!setSocketOpt(IPPROTO_TCP, TCP_NODELAY, val)) {
            CLOG_ERROR() << "Socket Set TCP_NODELAY Error";
          }
        }
    }
    getLocalAddress();
    getPeerAddress();
    return true;
}

Socket::ptr Socket::CreateUdpSocket() {
  Socket::ptr sock = std::make_unique<Socket>(Address::Protocol::IPv4, Address::Type::UDP);
  sock->newSocket();
  sock->isConnected_ = true;
  return sock;
}

Socket::ptr Socket::CreateTcpSocket() {
  Socket::ptr sock = std::make_unique<Socket>(Address::Protocol::IPv4, Address::Type::TCP);
  sock->newSocket();
  sock->isConnected_ = true;
  return sock;
}

bool Socket::checkConnected() {
    struct tcp_info info;
    int len = sizeof(info);
    getSocketOpt(IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    isConnected_ = (info.tcpi_state == TCP_ESTABLISHED);
    return isConnected_;
}

int Socket::connect(const Address::ptr addr) {
  remoteAddress_ = addr;
  int ret = ::connect(fd_.get(), addr->addr(), *(addr->addrLen()));
  isConnected_ = true;
  getPeerAddress();
  getLocalAddress();
  return ret;
}

bool Socket::isInvalid() { return this->fd_.isNull(); }

bool Socket::isConnected() const { return isConnected_;}

bool Socket::bind(const Address::ptr addr) {
  int ret = ::bind(fd_.get(), addr->addr(), *(addr->addrLen()));
  if (ret < 0) {
    fd_.close();
    return false;
  }
  getLocalAddress();
  return true;
}

bool Socket::listen() {
  int ret = ::listen(fd_.get(), BACK_LOG);
  if (ret < 0) {
    fd_.close();
    return false;
  }
  return true;
}

ssize_t Socket::send(const void* buffer, size_t length, int flags) {
    if(isConnected()) {
        return ::send(fd_.get(), buffer, length, flags);
    }
    return -1;
}

Socket::ptr Socket::accept() {
  int newSocket= ::accept(fd_.get(),nullptr,nullptr);
  //这里实例化一个对端socket
  Socket::ptr sock = std::make_unique<Socket>(protocol_,type_);
  if (newSocket < 0) {
    fd_.close();
    return nullptr;
  }
  return sock;
}

int Socket::getFd() { return fd_.get(); }

void Socket::close() { 
  if(fd_.isNull()){
    return;
  }
  else{
    isConnected_= false;
    fd_.close(); 
  }
}

void Socket::shutDownWrite() {
  if (::shutdown(fd_.get(), SHUT_WR) < 0) {
    CLOG_ERROR() << "Socket ShutDown Error";
  }
}
Address::ptr Socket::getLocalAddress(){
    if(localAddress_) {
        return localAddress_;
    }
    sockaddr_in m_addr;
    sockaddr* addr = (sockaddr*)&m_addr;
    socklen_t addrLen_ = sizeof(sockaddr);
    if(::getsockname(fd_.get(),addr,&addrLen_)){
      return nullptr;
    }
    char ip[32];
    inet_ntop(protocol_, &m_addr.sin_addr, ip, sizeof(ip));
    uint16_t port  = ntohs(m_addr.sin_port);
    Address::ptr res = Address::ptr(new Address(ip,port));
    localAddress_ = res;
    return  localAddress_;
}

Address::ptr Socket::getPeerAddress(){
    if(remoteAddress_) {
        return remoteAddress_;
    }
    sockaddr_in m_addr;
    sockaddr* addr = (sockaddr*)&m_addr;
    socklen_t addrLen_ = sizeof(sockaddr);
    if(::getpeername(fd_.get(),addr,&addrLen_)){
      CLOG_ERROR() << "Get Peer Address Error";
      return nullptr;
    }
    char ip[32];
    inet_ntop(protocol_,&m_addr.sin_addr, ip, sizeof(ip));
    uint16_t port  = ntohs(m_addr.sin_port);
    Address::ptr res = Address::ptr(new Address(ip,port));
    remoteAddress_ = res;
    return  remoteAddress_;
}

bool Socket::isSelfConnect()
{
    Address::ptr local_addr =  getLocalAddress();
    Address::ptr peer_addr  =  getPeerAddress();
    if(local_addr->addr()->sa_family == AF_INET){
        const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(local_addr->addr());  
        const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(peer_addr->addr());   
        return laddr4->sin_port == raddr4->sin_port
        && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    }
    else{
      //IPV6 TUDO
      return false;
    }
}

bool Socket::getSocketOpt(int level, int opt_name, void *opt_val,
                          socklen_t *opt_len) {
  if (::getsockopt(fd_.get(), level, opt_name, opt_val, opt_len) != 0) {
    return false;
  }
  return true;
}

bool Socket::setSocketOpt(int level, int opt_name, int opt_val) {
  if (::setsockopt(fd_.get(), level, opt_name, &opt_val, sizeof(opt_val)) !=
      0) {
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

bool Socket::setRecvBufferSize(int size) {
  return setSocketOpt(SOL_SOCKET, SO_RCVBUF, size);
}

bool Socket::setSendBufferSize(int size) {
  return setSocketOpt(SOL_SOCKET, SO_SNDBUF, size);
}

bool Socket::setKeepAlive(bool enable)
{
  if(enable) {
      int keepalive = 1; 
      if(!setSocketOpt(SOL_SOCKET, SO_KEEPALIVE, keepalive)){ 
        return false;
      }
      if(!setSocketOpt(SOL_TCP, TCP_KEEPIDLE,KEEP_IDLE)) {
        return false;
      }
      if(!setSocketOpt(SOL_TCP, TCP_KEEPINTVL, KEEP_INTEL)){
        return false;
      }
      if(setSocketOpt(SOL_TCP, TCP_KEEPCNT,KEEP_CNT ))
      {
        return false;
      }
      return true;
  }
  else {
    int keepalive = 0; 
    return setSocketOpt(SOL_SOCKET, SO_KEEPALIVE,keepalive);
  }
}
