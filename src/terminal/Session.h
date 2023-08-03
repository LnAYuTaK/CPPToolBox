#pragma once
#include "Types.h"
class Connection;
class Session {
 public:
  Session(Connection *conn, const SessionToken &st);

  bool send(char ch) const;
  bool send(const std::string &str) const;
  bool endSession() const;
  bool isValid() const;

 private:
  Connection *conn_;
  const SessionToken st_;
};
using Func = std::function<void(const Session &s, const Args &)>;
