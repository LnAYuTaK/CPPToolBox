#pragma once 

#include <string>
#include <vector>
#include <string>
#include <functional>

#include "Cabinet.h"
#include "Token.h"

using NodeToken    = Token;
using PathItem  = std::pair<std::string, NodeToken>;
using Path      = std::vector<PathItem>;
using SessionToken = Token;
using Args = std::vector<std::string>;

enum class NodeType { kFunc, kDir };

struct NodeInfo {
    std::string name;
    NodeToken   token;

    NodeInfo(const std::string &n, const NodeToken &t) :
        name(n), token(t) { }
};

