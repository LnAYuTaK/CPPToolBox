#pragma once 

#include "node.h"
#include  <vector>
#include "dir_node.h"
#include <string>
#include <iostream>
class SessionContext;

class Terminal{
  public:
    Terminal();
    ~Terminal();
    
    enum Option : uint32_t {
        kEnableEcho = 0x01,
        kQuietMode  = 0x02, //! 安静模式，不主动打印提示信息
    };
  public:
    /**********************************
          terminal   
    **********************************/
    SessionContext* newSession();
    bool deleteSession(SessionContext  *st);
    uint32_t getOptions(SessionContext  *st) const;
    void setOptions(SessionContext *st, uint32_t options);
    bool onBegin(SessionContext *st);
    bool onExit(SessionContext *st);
    bool onRecvString(SessionContext *st, const std::string &str);
    bool onRecvWindowSize(SessionContext *st, uint16_t w, uint16_t h);
    void printPrompt(SessionContext *s);
    void printHelp(SessionContext *s);


  public:
    /**********************************
          terminal_nodes   
    **********************************/
    Node* createFuncNode(const NodeFunc &func, const std::string &help);
    Node* createDirNode(const std::string &help);
    Node* rootNode() const;
    Node* findNode(const std::string &path) const;
    bool  findNode(const std::string &path, Path &node_path) const;
    //bool  mountNode(const Node &parent, const Node &child, const std::string &name);
    /**********************************
          terminal_key_events  
    **********************************/
    void onChar(SessionContext *s, char ch);
//     void onEnterKey(SessionContext *s);
    void onBackspaceKey(SessionContext *s);
    void onDeleteKey(SessionContext *s);
    void onTabKey(SessionContext *s);
    void onMoveUpKey(SessionContext *s);
    void onMoveDownKey(SessionContext *s);
    void onMoveLeftKey(SessionContext *s);
    void onMoveRightKey(SessionContext *s);
    void onHomeKey(SessionContext *s);
    void onEndKey(SessionContext *s);
    /**********************************
          terminal_key_events  
    **********************************/
    bool execute(SessionContext *s);
    bool executeCmd(SessionContext *s, const std::string &cmdline);
    void executeCdCmd(SessionContext *s, const Args &args);
//     void executeHelpCmd(SessionContext *s, const Args &args);
//     void executeLsCmd(SessionContext *s, const Args &args);
//     void executeHistoryCmd(SessionContext *s, const Args &args);
//     void executeExitCmd(SessionContext *s, const Args &args);
//     void executeTreeCmd(SessionContext *s, const Args &args);
//     void executePwdCmd(SessionContext *s, const Args &args);
//     bool executeRunHistoryCmd(SessionContext *s, const Args &args);
//     void executeUserCmd(SessionContext *s, const Args &args);
  private:
   std::vector<SessionContext*> sessions_;
   std::vector<Node*>  nodes_;
   //最顶端的Node
   Node *    rootNode_;   

};
