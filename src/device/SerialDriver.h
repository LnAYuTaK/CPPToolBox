/**
 * @file SerialDriver.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief 串口设备
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <functional>
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "IODevice.h"

using namespace itas109;
class SerialListener;
class SerialDirver : public IODevice {
 public:
  SerialDirver();
  ~SerialDirver() override;

  void init(const char *portName, int baudRate = BaudRate9600,
                    Parity parity = ParityNone, DataBits dataBits = DataBits8,
                    StopBits stopbits = StopOne,
                    FlowControl flowControl = FlowNone,
                    unsigned int readBufferSize = 4096);

  DriverType type() const override { return this->_type; }

  virtual void onReadEvent(const char *portName,
                           unsigned int readBufferLen) = 0;
  //读取串口数据
  int readData(void *data, int size) { return _port->readData(data, size); }
  //写入串口数据
  int writeData(const void *data, int size) {
    return _port->writeData(data, size);
  }
  //打开串口
  bool open()  { return _port->open(); }
  //关闭串口
  void close() override { _port->close(); }
   
  bool start()override{return true;}

  void stop() override{}

  void cleanup()override{}

 private:
  CSerialPort *_port;
  SerialListener *_listener;
  DriverType _type;
};

class SerialListener : public CSerialPortListener {
 public:
  SerialListener(CSerialPort *port, SerialDirver *driver);
  void onReadEvent(const char *portName, unsigned int readBufferLen) override;

 private:
  CSerialPort *_port;
  SerialDirver *_driver;
};

#define SERIAL_DRIVER_CLASS(classname)                     \
  class classname : public SerialDirver {                  \
   private:                                                \
   public:                                                 \
    classname() { ; }                                      \
    ~classname() { ; }                                     \
    void onReadEvent(const char *portName,                 \
                     unsigned int readBufferLen) override; \
  };
