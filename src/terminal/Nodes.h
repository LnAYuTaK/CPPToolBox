/**
 * @file Nodes.h
 * @author 刘宽 (807874484@qq.com)
 * @brief 节点负责终端显示与存储
 * 文件节点可以储存文件节点和方法节点
 * 方法节点不能负责存储 只能异步执行方法
 * @version 0.1
 * @date 2023-08-02
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <map>
#include <string>

#include <string>
#include "Session.h"

class Node {
 public:
  explicit Node(const std::string &help) : help_(help) {}
  virtual ~Node() {}

  virtual NodeType type() const = 0;
  std::string help() const { return help_; }

 private:
  std::string help_;
};

class DirNode : public Node {
 public:
  using Node::Node;

  virtual NodeType type() const override { return NodeType::kDir; }

  bool addChild(const NodeToken &nt, const std::string &child_name);
  NodeToken findChild(const std::string &child_name) const;
  void children(std::vector<NodeInfo> &vec) const;

 private:
  std::map<std::string, NodeToken> children_;
};

class FuncNode : public Node {
 public:
  FuncNode(const Func &func, const std::string &help);

  virtual NodeType type() const override { return NodeType::kFunc; }
  void execute(const Session &s, const Args &a) const;

 private:
  Func func_;
};
