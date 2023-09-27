#include "TcpClient.h"

#include <functional>

#include "CLog.h"
#include "MacroDef.h"

#include "Socket.h"
#include "Connector.h"

TcpClient::TcpClient(Loop *loop,const Address::ptr addr)
    : loop_(loop)
    , connector_(std::make_unique<Connector>(loop_,addr)) 
    , state_(State::None) 
    , recon_enable_(false) { 
  connector_->setNewConnectionCallback(
  std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
  state_ = State::Inited;
}

TcpClient::~TcpClient() { 
  CLOG_INFO() << "DELETE";
  cleanup(); }

bool TcpClient::start() {
  state_ = State::Connecting;
  return connector_->start();
}

void TcpClient::stop() {
  //如果是正在连接状态直接停止连接器就行
  if (state_ == State::Connecting) {
    connector_->stop();
  }
  //如果是已连接状态断开connection的连接
  else if (state_ == State::Connected) {
    connector_->stop();
    if (connection_) {
      connection_->disconnect();
    }
  }
  state_ = State::Inited;
}

void TcpClient::cleanup() {
  stop();
}

TcpClient::State TcpClient::state() const { return this->state_; }

void TcpClient::setConnectCallback(const ConnectCallback &&cb) {
  connected_cb = std::move(cb);
}

void TcpClient::setReceiveCallback(const ReceiveCallback &&cb) {
  receive_cb = std::move(cb);
}

void TcpClient::setCloseCallback(const CloseCallback &&cb) {
  close_cb = std::move(cb);
}

void TcpClient::setAutoReconnect(bool enable){
    recon_enable_ = enable;
}

void TcpClient::newConnection(Socket::ptr &sock_ptr) {
  //创建一个新的连接
  connection_ = make_shared<TcpConnection>(loop_, sock_ptr);
  //重新加载一下回调
  connection_->setReceiveCallback(receive_cb);
  // connection_->setCloseCallback(close_cb);
  connection_->setCloseCallback(std::bind(&TcpClient::OnTcpDisConnected,this));
  connection_->setConnectionCallback(connected_cb);
  connection_->connectFinished();
  state_ = State::Connected;
}

void TcpClient::OnTcpDisConnected(){
   if(recon_enable_){
      connector_->reStart();
   }
   if(close_cb){
     close_cb(connection_);
   }
}
