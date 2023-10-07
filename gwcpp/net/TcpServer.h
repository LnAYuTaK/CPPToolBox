/**
 * @file TcpServer.h
 * @author 刘宽 (807874484@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-26
 * 
 * @copyright Copyright (c) 2023 国网中电研发部
 * 
 */
#pragma once 

#include <memory>
#include <map>

#include "Address.h"
#include "Acceptor.h"

#include "gwcpp/event/Loop.h"

namespace gwcpp
{

class TcpServer
{
    DISALLOW_COPY_AND_ASSIGN(TcpServer)
public:
    using ConnectCallback = std::function<void(const TcpConnection::ptr &)>;
    using ReceiveCallback = std::function<void(const TcpConnection::ptr &, void *, ssize_t)>;
    using CloseCallback   = std::function<void(const TcpConnection::ptr &)>;

    using ConnectionMap   = std::map<string, TcpConnection::ptr>;

    TcpServer(Loop *loop,const Address::ptr address);
    ~TcpServer();
    /**
     * @brief 服务器！启动
     * 
     * @param aysn_start 
     * @return true 
     * @return false 
     */
    bool start(bool aysn_start = false);
    /**
     * @brief  设置新连接回调
     * 
     * @param cb 
     */
    void setConnectCallback(const ConnectCallback& cb);
    /**
     * @brief Set the Receive Callback object
     * 
     * @param cb 
     */
    void setReceiveCallback(const ReceiveCallback& cb);
    /**
     * @brief Set the Close Callback object
     * 
     * @param cb 
     */
    void setCloseCallback(const CloseCallback &cb);  
 private:
    /**
     * @brief 新的连接
     * 
     * @param socket 
     */
    void newConnection(Socket::ptr &socket);
    /**
     * @brief 移除新的链接
     * 
     * @param conn 
     */
    void removeConnection(const TcpConnection::ptr & conn);

private:
    Loop *loop_;
    Acceptor::ptr acceptor_;
    ConnectCallback conn_cb_;
    ReceiveCallback recv_cb_;
    CloseCallback   close_cb_;
    ConnectionMap connections_;
    bool started_;
    Address::ptr address_;
    int connId_;
};
 
} // namespace gwcpp
