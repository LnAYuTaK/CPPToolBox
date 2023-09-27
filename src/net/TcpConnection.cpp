#include "TcpConnection.h"

#include "ByteBuf.h"
#include "CLog.h"
#include "EpollFdEvent.h"
#include "Event.h"
#include "Loop.h"
#include "TcpClient.h"

constexpr static int TEMP_BUF_SIZE = 2048;

TcpConnection::TcpConnection(Loop *loop, Socket::ptr &socket)
    : loop_(loop),
      state_(State::Connecting),
      socket_(std::move(socket)),
      event_(Loop::CreatFdEvent(loop, "TcpConnection")),
      connected_cb(nullptr),
      receive_cb_(nullptr),
      close_cb_(nullptr),
      read_buffer_(std::make_unique<ByteBuf>()),
      write_buffer_(std::make_unique<ByteBuf>()) {
  event_->init(socket_->getFd(), Event::Mode::kPersist);
  event_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
  event_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  event_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
  event_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
  socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
}

void TcpConnection::send(const void *message, int len) {
  if (state_ == State::Connected) {
    if (!event_->isWriting()) {
      event_->enableWriting();
    }
    write_buffer_->writeBytes(static_cast<const char *>(message), len);
  }
}

void TcpConnection::send(ByteBuf &message) {
  if (state_ == State::Connected) {
    if (!event_->isWriting()) {
      event_->enableWriting();
    }
    write_buffer_->writeBytes(message);
  }
}

void TcpConnection::disconnect() {
  if (state_ == Connected || state_ == Disconnecting) {
    handleClose();
  }
}

void TcpConnection::setConnectionCallback(const ConnectCallback &cb) {
  connected_cb = cb;
}

void TcpConnection::setReceiveCallback(const ReceiveCallback &cb) {
  receive_cb_ = cb;
}
void TcpConnection::setCloseCallback(const CloseCallback &cb) { 
  close_cb_ = cb; 
}

void TcpConnection::connectFinished() {
  if (state_ == State::Connecting) {
    state_ = State::Connected;
    event_->enableReading();
    if (connected_cb) {
      connected_cb(shared_from_this());
    }
  } else {
    CLOG_ERROR() << "TcpConnection Enable Error";
  }
}

void TcpConnection::handleWrite() {
  if (state_ == State::Connected) {
    if (event_->isWriting()) {
      ByteBuf buf = write_buffer_->readBytes(write_buffer_->readableBytes());
      //当可读字节为0 的时候发送完成
      ssize_t curr_write = socket_->send((void *)(buf.data()),
                                          static_cast<size_t>(buf.readableBytes()), 
                                          0);
      //剩余字节为0 缓冲区写入完成//
      if (write_buffer_->readableBytes() == 0) {
        event_->disableWriting();
      }
      //如果剩余字节大于0 等待下次写入吧//
    }
  }
  //未连接状触发了HandleWrite,应该不可能，为了安全还是清空下缓存区
  else {
    //但是还是清理一下防止堆积数据
    write_buffer_->clear();
  }
}

void TcpConnection::handleClose() {
  CLOG_ERROR() << "TcpConnection Handle CLOSE";
  //服务器检测断开//
  assert(state_ == Connected || state_ == Disconnecting);
  state_ = State::Disconnected;
  event_->disableAll();
  if (close_cb_) {
    close_cb_(shared_from_this());
  }
  //销毁事件//
  event_->remove();
}

void TcpConnection::handleRead() {
  auto endReadCall = [this]() {
    if (receive_cb_) {
      receive_cb_(shared_from_this(), (void *)(read_buffer_->data()),
                  static_cast<ssize_t>(read_buffer_->readableBytes()));
    }
  };
  char buf[TEMP_BUF_SIZE];
  bzero(buf, sizeof(buf));
  while (true) {
    ssize_t curr_read = ::recv(socket_->getFd(), buf, TEMP_BUF_SIZE, 0);
    if (curr_read > 0) {
      read_buffer_->writeBytes(buf, curr_read);
      bzero(buf, sizeof(buf));
      endReadCall();
      read_buffer_->clear();
      break;
    } else if (curr_read == 0) {
      //服务器关闭了//
      handleClose();
      break;
    } else {
      handleError();
      break;
    }
  }
}

void TcpConnection::handleError() {
  //TCP 错误事件//
  CLOG_ERROR() << "TcpConnection Handle Error";
}
