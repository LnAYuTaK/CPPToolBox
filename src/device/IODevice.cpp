#include "IODevice.h"
#include "ADCDevice.h"
#include "EpollLoop.h"
#include "SerialDevice.h"

SerialDevice* IODevice::creatSerialDevice(EpollLoop* loop,
                                          const std::string& name) {

  
  return new SerialDevice(loop, name);
}