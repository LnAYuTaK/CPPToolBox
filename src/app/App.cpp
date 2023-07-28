
#include "App.h"
#include "terminal.h"
#include "session_context.h"
//实例化模块
Application::Application() 
                          :loop_(Loop::New()){
}
Application* app(void) { return Application::Instance(); }

Application::~Application(){ loop_->exitLoop(std::chrono::milliseconds(5));}

void Application::init() {}

void Application::exec(){
   this->loop_->runLoop(Loop::Mode::kForever);
}

void Application::start() {
   
  Terminal terminal;
  auto session  =  terminal.newSession();

  auto session2  = terminal.newSession();

  auto session3  = terminal.newSession();

  std::cout << session->id() << std::endl;
  std::cout << session2->id()<< std::endl;
  std::cout << session3->id()<< std::endl;

  

  terminal.printHelp(session);
  terminal.printHelp(session2);
  terminal.printHelp(session3);


}
