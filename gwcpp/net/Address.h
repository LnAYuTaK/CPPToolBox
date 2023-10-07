/**
 * @file Address.h
 * @author 刘宽 (807874484@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <memory>
#include <cstring>
#include <iostream>
#include <string>

namespace gwcpp
{

class Address {
 public:

  using ptr =std::shared_ptr<Address> ; 
  enum Protocol {
    // IPv4 socket
    IPv4 = AF_INET,
    // IPv6 socket
    IPv6 = AF_INET6,
    // Local socket
    Local = AF_LOCAL,
  };
  enum Type {
    /// TCP类型
    TCP = SOCK_STREAM,
    /// UDP类型
    UDP = SOCK_DGRAM
  };

  Address(Protocol protocol = Protocol::IPv4);
  Address(const char *ip, uint16_t port, Protocol protocol = Protocol::IPv4);
  ~Address() {}

  Address(const Address &other);
  Address &operator=(const Address &other);
  Address(Address &other);
  Address &operator=(Address &other);
  
  inline Protocol protocol();

  struct sockaddr *addr() ;

  socklen_t *addrLen();

  std::string ip() const;

  uint16_t port() const;

 private:
  //默认IPV4
  Protocol protocol_{Protocol::IPv4};
  mutable struct sockaddr_storage addr_ {};
  socklen_t addrLen_;
};

  
} // namespace gwcpp
