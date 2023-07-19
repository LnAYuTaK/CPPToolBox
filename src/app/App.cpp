
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

  
  
  // auto uart1 = IODevice::creatSerialDevice(loop_,"UART1");
  // if(uart1->init("/dev/ttyS4"))
  // {
  //   uart1->setReadCallback([&](const char *data, int dataLen){
  //         ByteBuf buf(data,dataLen);
  //         std::cout << buf.data() << "  "<< dataLen  <<std::endl;
  //   });
  // }
  // uart1->start();
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
