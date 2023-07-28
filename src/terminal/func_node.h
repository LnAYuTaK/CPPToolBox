#pragma once 

#include "node.h"
#include <map>
#include <functional>
#include "session.h"
class FuncNode : public Node  
{
  public:
    FuncNode(const NodeFunc  &func, const std::string &help);
    NodeType type() const override {
      return NodeType::kFunc;
    }
    void execute(const Session &s, const Args &a) const;
  private:
    NodeFunc  func_;
};





