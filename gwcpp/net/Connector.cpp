#include "Connector.h"

#include <sys/un.h>

using namespace gwcpp;
Connector::Connector(Loop *loop,const Address::ptr address)
    : loop_(loop)
    ,  state_(State::Disconnected)
    ,  try_times_(0)
    ,  delay_sec_(0)
    ,  connect_(false)
    ,  conn_forever(false) {
  addr_ = address;  
}
Connector::~Connector() { cleanup(); }

bool Connector::start(bool async_connect) {
  //只有未链接空闲状态可以重新Start
  //创建Socket
    if(state_ != State::Disconnected) {
      return false;
    }
    Socket::ptr new_socket = Socket::CreateTcpSocket();
    if (new_socket->isInvalid()) {
      return false;
    }
    CLOG_DEBUG() << "Ready Connect:Server addr: " << addr_->ip() << ":"
                << addr_->port();
    // socket连接//
    int conn_ret = new_socket->connect(addr_);
    //连接失败//
    if (conn_ret != 0) {
      new_socket->close();
      CLOG_INFO() << strerror(errno);
      return false;
    }
    //连接成功
    else {
      //保证重复连接会释放
      sock_ptr = std::move(new_socket);
      event_.reset(new EpollFdEvent(loop_,"Connector"));
      //设置 连接回调
      event_->init(sock_ptr->getFd(), Event::Mode::kPersist);
      event_->setWriteCallback(std::bind(&Connector::handleWrite, this));
      event_->enableWriting();
      state_ = State::Connecting;
      return true;
    }
}

void Connector::reStart()
{
  state_ = State::Disconnected;
  connect_ = true;
  start();
}

void Connector::stop() {
  connect_ = false;
  if (state_ == State::Connecting){
      state_ == State::Disconnected;
      removeAndResetChannel();
  }
}

void Connector::setAutoReconnect(bool enable){
  this->conn_forever = enable;
}

int Connector::removeAndResetChannel()
{
  event_->disableAll();
  event_->remove();
  int sockfd = event_->fd();
  loop_->runTask([this](){
    this->resetEvent();
  },false);
  return sockfd;
}

void Connector::resetEvent()
{
   event_.reset();
}
void Connector::cleanup() {
  stop();
}

Connector::State Connector::state() const { return state_; }

void Connector::setNewConnectionCallback(const NewConnectCallback &&cb) {
  connect_cb_ = cb;
}

//这里是接收到了socket可以写//可以创建连接了
void Connector::handleWrite() {
  //如果是正在连接状态
  if (state_ == State::Connecting) {
    int sock_errno = 0;
    int sockfd = removeAndResetChannel();
    socklen_t optlen = sizeof(sock_errno);
    sock_ptr->getSocketOpt(SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
    if(sock_errno) {
      CLOG_WARN()<< "Connector::handleWrite - Socket Error";
      reStart();
    }
    else if (sock_ptr->isSelfConnect()) {
      CLOG_WARN()<< "Connector::handleWrite - Self connect";
      reStart();
    }
    //正常连接//
    else{
      state_ = State::Connected;
      //连接事件清理//
      if (connect_cb_) {
        connect_cb_(sock_ptr);
      }
      else {
        sock_ptr->close();
      }
    }
  }
}

