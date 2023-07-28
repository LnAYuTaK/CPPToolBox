#pragma once 

#include "node.h"
#include <map>
#include <string>
#include <vector>

class DirNode : public Node {
  public:
    using Node::Node;

    NodeType type() const override { return NodeType::kDir; }
    
    bool   addChild(const  Node &nt, const std::string &child_name);
    Node*  findChild(const std::string &child_name) const;
    void   children(std::vector<NodeInfo> &vec) const;

  private:
    std::map<std::string, Node*> children_;
};

