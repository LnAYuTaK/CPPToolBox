
#include "App.h"
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

  #ifdef SERIAL_TEST
  Serial Port
  uart1 = IODevice::creatSerialDevice(loop_,"UART1");
  if(uart1->init("/dev/ttyS4"))
  {
    uart1->setReadCallback([](const char *data, int dataLen){
          ByteBuf buf(data,dataLen);
          std::cout << buf.data() << "  "<< dataLen  <<std::endl;
    });
  }
  uart1->start();
  #endif

  #ifdef TIMER_TEST
  auto timer =  loop_->creatTimerEvent("Timer1");
  timer->init(std::chrono::seconds(1), std::chrono::seconds(2));
  timer->setTimerCallback([](){
     CLOG_INFO() << "Timer Up";
     CLOG_INFO() << "Timer End";
    //  app()->uart1->send(buffer,sizeof(buffer));
  });
  timer->start();
  #endif

  #ifdef TASK_TEST
  loop_->runTask([](){
    CLOG_INFO() << "Task INFO1";
    THREAD_SLEEP_MS(2000);
    CLOG_INFO() << "Task INFO1";
  },false);

    loop_->runTask([](){
    CLOG_INFO() << "Task INFO2";
  },true);
  #endif
  
  #ifdef ADC_TEST
  ADCDevice ads1115;
  ads1115.init("/dev/i2c-2");
  ads1115.start();
  float s= 0 ;
  for (int i = 0; i < 20; i++)
  {
     ads1115.ads1115Read(&s);
     CLOG_INFO() << s;
     THREAD_SLEEP_MS(200);
  }
  #endif
}
