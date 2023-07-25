
#pragma once 

#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
class NetAddress {
 public:
  enum class Protocol { Ipv4, Ipv6 };

  NetAddress(const char *ip, uint16_t port, Protocol protocol = Protocol::Ipv4);

  ~NetAddress();

  inline Protocol protocol()const;

  struct sockaddr * addr()const;

  socklen_t * addrLen()  ;

  std::string  ip() const ;

  uint16_t port() const ;
private:
    Protocol protocol_{Protocol::Ipv4};
    mutable struct sockaddr_storage addr_ {};
    socklen_t  addrLen_;
};

