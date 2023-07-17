#pragma once 

#include "Module.h"
#include  "EpollFdEvent.h"
#include  "IODevice.h"
#include "EpollLoop.h"
#include <iostream>

class UartDevice  : public  IODevice
{
public:

  UartDevice (EpollLoop *loop,std::string &name);
  ~UartDevice ()override;
  bool init()
  {
        return true;
  }

  bool  start()override
  {
    return true;
  }

  void  stop() override
  {

  }

  void  close() override
  {

  }

  void cleanup() override
  {

  }


    
private: 
    EpollFdEvent  *  uartDevice;
};
