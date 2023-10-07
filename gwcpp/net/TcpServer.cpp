#include "TcpServer.h"
using namespace gwcpp;

TcpServer::TcpServer(Loop *loop,const Address::ptr address)
    : loop_(loop)
    , acceptor_(new Acceptor(loop_,address))
    , started_(false)
    , address_(address)
    , connId_(1)
{
   acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection,this, std::placeholders::_1)
   );
}

bool TcpServer::start(bool aysnc_start) {
    if(!acceptor_->start(aysnc_start)){
      return false;
    }
    started_ = true;
    return true;
} 

TcpServer::~TcpServer(){

}

void TcpServer::setConnectCallback(const ConnectCallback& cb) {
    conn_cb_ = cb;   
}

void TcpServer::setReceiveCallback(const ReceiveCallback& cb) {
    recv_cb_ = cb;   
}

void  TcpServer::setCloseCallback(const CloseCallback &cb) {
    close_cb_ = cb;  
}

void TcpServer::newConnection(Socket::ptr &socket) {
  char buf[64];
  bzero(buf,sizeof(buf));
  snprintf(buf, sizeof(buf),"%s-%d", (address_->ip()+"-"+std::to_string(address_->port())).c_str(),connId_);
  std::string connName = buf;

  TcpConnection::ptr conn(new TcpConnection(loop_,socket,connName));
  connections_[connName] = conn;
  conn->setConnectionCallback(conn_cb_);
  conn->setReceiveCallback(recv_cb_);
  conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
  conn->connectFinished();
}

void TcpServer::removeConnection(const TcpConnection::ptr & conn) {
  //清理下 Map里的链接//
   size_t n = connections_.erase(conn->name());
   loop_->runTask(std::bind(&TcpConnection::disconnect, conn),false);
  if(close_cb_){
     close_cb_(conn);
  }
}

