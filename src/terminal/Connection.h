#pragma once 
#include "types.h"

class Connection {
  public:
    virtual bool send(const SessionToken &st, char ch) = 0;
    virtual bool send(const SessionToken &st, const std::string &str) = 0;
    virtual bool endSession(const SessionToken &st) = 0;
    virtual bool isValid(const SessionToken &st) const = 0;

  protected:
    virtual ~Connection() { }
};
