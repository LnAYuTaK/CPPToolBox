#include "TcpClient.h"

#include <functional>

using namespace gwcpp;

TcpClient::TcpClient(Loop *loop,const Address::ptr addr)
    : loop_(loop)
    , address_(addr)
    , connector_(new Connector(loop_,addr)) 
    , state_(State::None)
    , connId_(1)
    , recon_enable_(false) { 
  connector_->setNewConnectionCallback(
  std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
  state_ = State::Inited;
}

TcpClient::~TcpClient() { 
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

void TcpClient::setConnectCallback(const ConnectCallback &cb) {
  connected_cb = cb;
}

void TcpClient::setReceiveCallback(const ReceiveCallback &cb) {
  receive_cb = cb;
}

void TcpClient::setCloseCallback(const CloseCallback &cb) {
  close_cb = cb;
}

void TcpClient::setAutoReconnect(bool enable){
    recon_enable_ = enable;
}

void TcpClient::newConnection(Socket::ptr &sock_ptr) {
  //创建一个新的连接
  char buf[64];
  bzero(buf,sizeof(buf));
  snprintf(buf, sizeof(buf),"%s-%d", (address_->ip() +std::to_string(address_->port())).c_str(),connId_);
  std::string connName = buf;
  connection_ = TcpConnection::ptr(new TcpConnection(loop_, sock_ptr,connName));
  //重新加载一下回调
  connection_->setReceiveCallback(receive_cb);
  connection_->setCloseCallback(std::bind(&TcpClient::removeConnection,this,std::placeholders::_1));
  connection_->setConnectionCallback(connected_cb);
  connection_->connectFinished();
  state_ = State::Connected;
}

void TcpClient::removeConnection(const TcpConnection::ptr& conn) {

  connection_.reset();
  //
  loop_->runTask(std::bind(&TcpConnection::disconnect, conn),false);
  if (recon_enable_&& (state_== State::Connected)) {
    connector_->reStart();
  }
  if(close_cb){
     close_cb(connection_);
  }
}

