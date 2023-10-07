
#include "App.h"
#include "Connection.h"
#include "Acceptor.h"

#include "TcpClient.h"
#include "TcpServer.h"
#include "Terminal.h"

//实例化模块
Application::Application() : loop_(Loop::CreatLoop()) {}
Application *app(void) { return Application::Instance(); }

Application::~Application() { loop_->exitLoop(std::chrono::milliseconds(5)); }

void Application::init() {}
void Application::exec() { this->loop_->runLoop(Loop::Mode::kForever); }

void Application::start() {
//   Address::ptr addr(new Address("192.168.149.62",2345));  
//   TcpClient * tcpClient = new TcpClient(loop_,addr);
//   tcpClient->setReceiveCallback([](const TcpConnection::ptr &conn, void *data, ssize_t  n){
//     char buffer[256];
//     bzero(buffer, 256);
//     memcpy(buffer, (char *)data, n);
//     CLOG_INFO() << "Client recv: " << buffer;
//     conn->send(buffer, n);
//   });
//   tcpClient->start();
//   tcpClient->setAutoReconnect(true);
// }
  Address::ptr addr(new Address("192.168.149.190",2345));  
  TcpServer *tcpServer = new TcpServer(loop_,addr);
  tcpServer->setConnectCallback([](const TcpConnection::ptr &conn){
        CLOG_INFO() << conn->name();
  });
  tcpServer->setReceiveCallback([](const TcpConnection::ptr &conn, void *data, ssize_t  n){
    char buffer[256];
    bzero(buffer, 256);
    memcpy(buffer, (char *)data, n);
    CLOG_INFO() << "Client recv: " << buffer;
  });
  tcpServer->start();
}
