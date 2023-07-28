#include "dir_node.h"

bool DirNode::addChild(const Node &nt, const std::string &child_name)
{
    auto iter = children_.find(child_name);
    if (iter != children_.end()){
        return false;
    }
    return true;
}

Node* DirNode::findChild(const std::string &child_name) const
{
    auto iter = children_.find(child_name);
    if (iter == children_.end()){
        return nullptr;
    }
    return iter->second;
}

void DirNode::children(std::vector<NodeInfo> &vec) const
{
    for (auto item : children_) {
        NodeInfo info(item.first,item.second);
        vec.push_back(info);
    }
}

