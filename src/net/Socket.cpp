#include "Socket.h"
#include "NetAddress.h"

static constexpr int BACK_LOG = 128;
Socket::Socket(const Fd &fd) :
    Fd(fd)
{ }

Socket Socket::createSocket(int domain, int type, int protocal) {
    int fd = ::socket(domain, type, protocal);
    if (fd < 0) {
        CLOG_ERROR() << "Socket Error";
        return Socket(-1);
    }
    return Socket(fd);
}

Socket Socket::createUdpSocket() {
    return createSocket(AF_INET, SOCK_DGRAM, 0);
}

Socket Socket::createTcpSocket() {
    return createSocket(AF_INET, SOCK_STREAM, 0);
}

int Socket::connect(NetAddress &adress) {
    int ret = ::connect(get(),adress.addr(),*(adress.addrLen()));
    if (ret < 0){
       CLOG_ERROR() << "Connect Error";
    }
    return ret;
}

int Socket::bind(NetAddress &adress) {
    int ret = ::bind(get(),adress.addr(),*(adress.addrLen()));
    if (ret < 0){
            CLOG_ERROR() << "Bind Error";
    }
    return ret;
}

int Socket::listen() {
    int ret = ::listen(get(), BACK_LOG);
    if (ret < 0) {
        CLOG_ERROR() << "Listen Error";
    }
    return ret;
}

int Socket::accept(NetAddress &adress) {
    int ret = ::accept(get(), adress.addr(),adress.addrLen());
    if (ret < 0) {
        CLOG_ERROR() << "Accept Error";
    }
    return ret;
}

bool Socket::getSocketOpt(int level, int optname, void *optval, socklen_t *optlen) {
    if (::getsockopt(get(), level, optname, optval, optlen) != 0) {
         CLOG_ERROR() << "Get Socket Opt Error";
        return false;
    }
    return true;
}

bool Socket::setSocketOpt(int level, int optname, int optval) {
    if (::setsockopt(get(), level, optname, &optval, sizeof(optval)) != 0) {
        return false;
    }
    return true;
}

bool Socket::setSocketOpt(int level, int optname, const void *optval, socklen_t optlen)
{
    if (::setsockopt(get(), level, optname, optval, optlen) != 0) {
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
