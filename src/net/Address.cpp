
#include "Address.h"
#include <sys/un.h>
Address::Address(Protocol protocol)
: protocol_(protocol){ 
  memset(&addr_, 0, sizeof(addr_)); 
  addrLen_ = sizeof(addr_);
}

Address::Address(const char *ip, uint16_t port, Protocol protocol)
    : protocol_(protocol) {
  memset(&addr_, 0, sizeof(addr_));
  addrLen_ = sizeof(addr_);
  if (protocol_ == Protocol::IPv4) {
    auto addr_ipv4 = reinterpret_cast<struct sockaddr_in *>(&addr_);
    addr_ipv4->sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr_ipv4->sin_addr.s_addr);
    addr_ipv4->sin_port = htons(port);
  } else if (protocol == Protocol::IPv6) {
    auto addr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(&addr_);
    addr_ipv6->sin6_family = AF_INET6;
    inet_pton(AF_INET6, ip, &addr_ipv6->sin6_addr.s6_addr);
    addr_ipv6->sin6_port = htons(port);
  } else if (protocol == Protocol::Local) {
    auto addr_local = reinterpret_cast<struct sockaddr_un *>(&addr_);
    addr_local->sun_family = AF_LOCAL;
  }
}

Address::Address(const Address &other) {
  addrLen_ = other.addrLen_;
  ::memcpy(&addr_, &other.addr_, addrLen_);
}

Address &Address::operator=(const Address &other) {
  if (this != &other) {
    addrLen_ = other.addrLen_;
    ::memcpy(&addr_, &other.addr_, addrLen_);
  }
  return *this;
}

Address::Address(Address &other) {
  addrLen_ = other.addrLen_;
  ::memcpy(&addr_, &other.addr_, addrLen_);
}

Address &Address::operator=(Address &other) {
  if (this != &other) {
    addrLen_ = other.addrLen_;
    ::memcpy(&addr_, &other.addr_, addrLen_);
  }
  return *this;
}

Address::Protocol Address::protocol(){ return protocol_; }

struct sockaddr *Address::addr() {
  return reinterpret_cast<struct sockaddr *>(&addr_);
}

socklen_t *Address::addrLen() { return &addrLen_; }

std::string Address::ip() const {
  char ip_address[INET6_ADDRSTRLEN];
  if (protocol_ == Address::Protocol::IPv4) {
    auto addr_ipv4 = reinterpret_cast<struct sockaddr_in *>(&addr_);
    inet_ntop(AF_INET, &addr_ipv4->sin_addr, ip_address, INET_ADDRSTRLEN);
  } else if (protocol_ == Address::Protocol::IPv6) {
    auto addr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(&addr_);
    inet_ntop(AF_INET6, &addr_ipv6->sin6_addr, ip_address, INET6_ADDRSTRLEN);
  }
  return ip_address;
}

uint16_t Address::port() const {
  uint16_t port;
  if (protocol_ == Protocol::IPv4) {
    auto addr_ipv4 = reinterpret_cast<struct sockaddr_in *>(&addr_);
    port = ntohs(addr_ipv4->sin_port);
  } else if (protocol_ == Address::Protocol::IPv6) {
    auto addr_ipv6 = reinterpret_cast<struct sockaddr_in6 *>(&addr_);
    port = ntohs(addr_ipv6->sin6_port);
  }
  return port;
}
