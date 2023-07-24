
#include "App.h"
/***********************************************************/
//实例化模块
Application::Application() 
                          :loop_(Loop::New()){

}
/***********************************************************/
Application* app(void) { return Application::Instance(); }
/***********************************************************/
Application::~Application() {}
/***********************************************************/
void Application::init() {
}
/***********************************************************/
void Application::start() {
  //
  //Serial Port
  // uart1 = IODevice::creatSerialDevice(loop_,"UART1");
  // if(uart1->init("/dev/ttyS4"))
  // {
  //   uart1->setReadCallback([](const char *data, int dataLen){
  //         ByteBuf buf(data,dataLen);
  //         std::cout << buf.data() << "  "<< dataLen  <<std::endl;
  //   });
  // }
  // uart1->start();
  //Timer !
  // auto timer =  loop_->creatTimerEvent("Timer1");
  // timer->init(std::chrono::seconds(2), std::chrono::seconds(2));
  // timer->setTimerCallback([](){
  //    CLOG_INFO() << "Timer Up";
  //   //  app()->uart1->send(buffer,sizeof(buffer));
  // });
  // timer->start();
  
  loop_->runTask([](){
    CLOG_INFO() << "Task INFO1";
  },true);

    loop_->runTask([](){
    CLOG_INFO() << "Task INFO2";
  },true);

    loop_->runTask([](){
    CLOG_INFO() << "Task INFO3";
  },false);
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
