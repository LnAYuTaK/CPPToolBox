
#include "Acceptor.h"

#include <sys/un.h>

using namespace gwcpp;

Acceptor::Acceptor(Loop *loop,const Address::ptr address) 
              : loop_(loop)
              , state_(State::Inited){
  addr_ = address;
}

Acceptor::~Acceptor() { cleanup(); }

bool Acceptor::start(bool async_connect) { 
  if(state_ == State::Inited){
    // Socket
    Socket::ptr new_socket = Socket::CreateTcpSocket();
    if (new_socket->isInvalid()) {
      return false;
    }
    // Bind
    if (!(new_socket->bind(addr_))) {
      CLOG_ERROR() << "Socket Bind Error";
      return false;
    }
    //Listen
    if (!(new_socket->listen())) {
      CLOG_ERROR() << "Socket Listen Error";
      return false;
    }
    acceptSocket_ = std::move(new_socket);
    event_ = EpollFdEvent::ptr(new EpollFdEvent(loop_, "Acceptor"));
    if (event_) {
      //设置 连接回调
      event_->init(acceptSocket_->getFd(), Event::Mode::kPersist);
      event_->setReadCallback(std::bind(&Acceptor::handleRead, this));
      event_->enableReading();
      state_ = State::Started;
      return true;
    }
    else{
      return false;
    }
  }
  else if(state_==State::Started){
    CLOG_WARN()<< "Ready Running";
    return true;
  }
  else if(state_==State::Stoped){
    if(event_) {
      event_->enableReading();
      state_ = State::Started;
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}
//停止
void Acceptor::stop() {
     if(state_ == State::Started){
        event_->disableReading(); 
        state_ == State::Stoped;
     }
}
//清理资源
void Acceptor::cleanup() {
  stop();
}
//
Acceptor::State Acceptor::state() const {
  return state_;
}

void Acceptor::handleRead() {
  if(event_){
    Socket::ptr socket = this->acceptSocket_->accept();
    if(socket && (socket->getFd()>0)){
      if(newConnectCb_) {
        newConnectCb_(socket);
      }
    }
    else{
      CLOG_ERROR() << "Socket Creat Error";
    }
  }
}

void Acceptor::setNewConnectionCallback(const NewConnectCallback &cb) {
    newConnectCb_  =  cb;
}
