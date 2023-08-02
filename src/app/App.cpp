
#include "App.h"
#include "Connection.h"
#include "Terminal.h"
//实例化模块
Application::Application() : loop_(Loop::New()) {}
Application *app(void) { return Application::Instance(); }

Application::~Application() { loop_->exitLoop(std::chrono::milliseconds(5)); }

void Application::init() {}

void Application::exec() { this->loop_->runLoop(Loop::Mode::kForever); }

void sync_func() { std::cout << "121212" << std::endl; }
void async_func() { std::cout << "121212" << std::endl; }

class ConnectionTest : public Connection {
 private:
  /* data */
 public:
  ConnectionTest() {}
  ~ConnectionTest() {}
  bool send(const SessionToken &st, char ch) override { return false; }
  bool send(const SessionToken &st, const std::string &str) override {
    std::cout << str;
    return false;
  }
  bool endSession(const SessionToken &st) override { return false; }
  bool isValid(const SessionToken &st) const override { return false; }
};

void Application::start() {
  Terminal term;
  ConnectionTest *test = new ConnectionTest();
  auto session = term.newSession(test);

  auto dir1_token = term.createDirNode("This is dir1");
  auto dir2_token = term.createDirNode("This is dir2");

  auto dir1_1_token = term.createDirNode("This is dir1_1");
  auto dir1_2_token = term.createDirNode("This is dir1_2");

  term.mountNode(dir1_token, dir1_1_token, "dir1_1");
  term.mountNode(dir1_token, dir1_2_token, "dir1_2");

  term.mountNode(term.rootNode(), dir1_token, "dir1");
  term.mountNode(term.rootNode(), dir2_token, "dir2");

  term.mountNode(dir1_1_token, term.rootNode(), "root");  //! 循环引用
  std::string input;
  while (std::cin >> input) {
    std::cout << input << std::endl;
    term.onRecvString(session, input.c_str());
  }
  /* code */
}
