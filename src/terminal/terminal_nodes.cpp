#include "terminal.h"
#include "util_string.h"
#include "dir_node.h"
#include "func_node.h"

Node *Terminal::createFuncNode(const NodeFunc &func, const std::string &help) {
    Node *node = new FuncNode(func, help);
    return node;
}

Node *Terminal::createDirNode(const std::string &help) {
    Node *node = new DirNode(help);
    return node;
}

Node* Terminal::rootNode() const {
    return rootNode_;
}

Node* Terminal::findNode(const std::string &path_str) const {

    Path node_path;
    if (findNode(path_str, node_path)) {
        if (node_path.empty())
            return rootNode_;
        else
            return node_path.back().second;
    } else {
        return nullptr;
    }
}

bool Terminal::findNode(const std::string &path_str, Path &node_path) const {
    std::vector<std::string> path_str_vec;
    Split(path_str, "/", path_str_vec);

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
            Node* top_node_token = node_path.empty() ? rootNode_: node_path.back().second;
            // Node *top_node = nodes_.at(top_node_token);
            auto it = std::find(nodes_.begin(), nodes_.end(), top_node_token);
            if (it == nodes_.end()){
                return false;
            }
            else{
                if ((*it)->type() == NodeType::kFunc){
                    return false;
                }
                else{   
                    DirNode *top_dir_node = static_cast<DirNode*>(top_node_token);
                    auto next_node_token = top_dir_node->findChild(node_name);
                    if (next_node_token == nullptr){
                        return false;
                    }
                    node_path.push_back(make_pair(node_name, next_node_token)); 
                }
            }
        }
    }
    return true;
}

// bool Terminal::mountNode(const Node& parent, const Node& child, const std::string &name) {
//     auto p_node = nodes_.at(parent);
//     auto c_node = nodes_.at(child);

//     if (p_node == nullptr || c_node == nullptr ||
//         p_node->type() != NodeType::kDir) {
//         return false;
//     }

//     if (name.empty() || name[0] == '!') {
//         return false;
//     }
//     auto p_dir_node = static_cast<DirNode*>(p_node);
//     return p_dir_node->addChild(child, name);
// }

