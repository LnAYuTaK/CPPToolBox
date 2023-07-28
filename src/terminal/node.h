#pragma once 

#include   <string>
#include   <map>
#include   <vector>
#include   "session.h"
#include   <functional>
enum  NodeType { kFunc, kDir };

class Node {
  public:
    explicit Node(const std::string &help) : help_(help) { }
    virtual ~Node() {}

    virtual NodeType type() const = 0;
    std::string help() const { return help_; }

  private:
    std::string help_;
};

class NodeInfo
{
public:
    NodeInfo(const std::string &n, Node *t)
            :name(n)
            ,token(t)
    {}
    ~NodeInfo(){
          this->token = nullptr;
    }
    std::string name;
    Node  *token;
};

using PathItem  = std::pair<std::string, Node*>;
using Path      = std::vector<PathItem>;
using Args      = std::vector<std::string>;
using NodeFunc  = std::function<void (const Session &s, const Args &)>;