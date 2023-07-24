/**
 * @file Socket.h
 * @author 刘宽 (807874484@qq.com)
 * @brief Socket
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023 国网中电研发部
 * 
 */
#pragma once 

#include "Fd.h"
#include "MacroDef.h"

class NetAddress;
enum class Protocol;
class Socket :public Fd 
{
public:
    Socket(int fd):Fd(fd){};
    ~Socket()override{};
    /**
     * @brief 连接
     * 
     * @param serverAddress 
     */
    void connect(NetAddress &serverAddress);
    /**
     * @brief 监听
     * 
     */
    void listen();

    /**
     * @brief 
     * 
     * @param client_address 
     * @return int 
     */
    int accept(NetAddress &client_address);
    /**
     * @brief 设置地址重用
     * 
     */
    void setReusable();

private:
    /* data */
};

