
#include "App.h"
/***********************************************************/
//实例化模块
Application::Application(/* args */) 
                          :loop_(Loop::New())
{

}
/***********************************************************/
Application* app(void) { return Application::Instance(); }
/***********************************************************/
Application::~Application() {}
/***********************************************************/
void Application::init(/* args */) {


}
/***********************************************************/
void Application::start() {

  auto uart1 = IODevice::creatSerialDevice(loop_,"UART1");
  CLOG_DEBUG()  << ": DEBUG2";
  uart1->init("/dev/ttyTHS1") ;

  //  auto fds = loop()->creatFdEvent("STDIO");
  //  fds->init(STDIN_FILENO,Event::Mode::kPersist);
  //  fds->setReadCallback([&](int fd){
  //     char buffer[256];
  //     bzero(buffer, sizeof(buffer));
  //     read(STDIN_FILENO, buffer, sizeof(buffer));
  //     CLOG_INFO() << buffer;
  //  });
  //  fds->enableReading();
  // ads1115.init("/dev/i2c-2");
  // ads1115.start();
  // float s= 0 ;
  // for (int i = 0; i < 20; i++)
  // {
  //    ads1115.ads1115Read(&s);
  //    CLOG_INFO() << s;
  //    THREAD_SLEEP_MS(200);
  // }
}
