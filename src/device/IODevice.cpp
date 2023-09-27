#include "IODevice.h"

#include "ADCDevice.h"
#include "Loop.h"
#include "SerialDevice.h"

SerialDevice* IODevice::CreatSerialDevice(Loop* loop, const std::string& name) {
  return new SerialDevice(loop, name);
}