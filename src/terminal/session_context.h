#pragma once 

#include <deque>
#include <memory>
#include <iostream>
#include <string>

#include "key_event_scanner.h"
#include "session.h"

class SessionContext
{
private:
    /* data */
public:
    SessionContext(int id)
        :options(0)
        ,curr_input("")
        ,cursor(0)
        ,history_index(0)
        ,window_width(0)
        ,window_height(0)
        ,id_(id)
    {
    }
    ~SessionContext()
    {
    }
    // Connection *wp_conn = nullptr;
    
    uint32_t options = 0;
    void send(const char* beginMsg)
    {
       std::cout << id() <<"  " << std::string(beginMsg) << std::endl;
    }
    void send(const std::string &beginMsg)
    {
       std::cout << id() <<"  "<< beginMsg << std::endl;
    }

    int id()const {return id_;}
    
    std::string curr_input;
    size_t cursor;
    //当前路径
    Path path; 
    //历史命令
    std::deque<std::string> history;   
    size_t history_index;  //! 0表示不指定历史命令

    KeyEventScanner key_event_scanner_;

    uint16_t window_width;
    uint16_t window_height;

    Session token;
private:
    int id_;

};

