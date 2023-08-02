#include "CLog.h"
#include "Connection.h"
#include "Nodes.h"
#include "SessionContext.h"
#include "Terminal.h"
#include "Types.h"
#include "Utils.h"

using namespace std;

NodeToken Terminal::createFuncNode(const Func &func, const string &help) {
  FuncNode *node = new FuncNode(func, help);
  return nodes_.alloc(node);
}

NodeToken Terminal::createDirNode(const string &help) {
  DirNode *node = new DirNode(help);
  return nodes_.alloc(node);
}

NodeToken Terminal::rootNode() const { return root_token_; }

NodeToken Terminal::findNode(const string &path_str) const {
  Path node_path;
  if (findNode(path_str, node_path)) {
    if (node_path.empty())
      return root_token_;
    else
      return node_path.back().second;
  } else {
    return NodeToken();
  }
}

bool Terminal::mountNode(const NodeToken &parent, const NodeToken &child,
                         const string &name) {
  auto p_node = nodes_.at(parent);
  auto c_node = nodes_.at(child);

  if (p_node == nullptr || c_node == nullptr ||
      p_node->type() != NodeType::kDir) {
    CLOG_WARN() << "mount: " << name.c_str()
                << "fail, parent or child is invalid";
    return false;
  }

  if (name.empty() || name[0] == '!') {
    CLOG_WARN() << "name: " << name.c_str() << "is invalid";
    return false;
  }

  auto p_dir_node = static_cast<DirNode *>(p_node);
  return p_dir_node->addChild(child, name);
}

bool Terminal::findNode(const string &path_str, Path &node_path) const {
  vector<string> path_str_vec;
  Utils::Split(path_str, "/", path_str_vec);

  //! 如果是以 '/' 开头的路径
  if (path_str_vec[0].empty()) {
    node_path.clear();
    path_str_vec.erase(path_str_vec.begin());
  }

  for (const auto &node_name : path_str_vec) {
    if (node_name == "." || node_name.empty()) {
      continue;
    } else if (node_name == "..") {
      if (node_path.empty())
        return false;
      else
        node_path.pop_back();
    } else {
      auto top_node_token =
          node_path.empty() ? root_token_ : node_path.back().second;
      Node *top_node = nodes_.at(top_node_token);
      if (top_node->type() == NodeType::kFunc) return false;

      DirNode *top_dir_node = static_cast<DirNode *>(top_node);
      auto next_node_token = top_dir_node->findChild(node_name);
      if (next_node_token.isNull()) return false;

      node_path.push_back(make_pair(node_name, next_node_token));
    }
  }
  return true;
}
