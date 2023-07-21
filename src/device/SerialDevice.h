/**
 * @file SerialDevice.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  串口外设
 * @version 0.1
 * @date 2023-07-20
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <iostream>
#include "EpollFdEvent.h"
#include "EpollLoop.h"
#include "Fd.h"
#include "IODevice.h"
#include "Module.h"
#include "MacroDef.h"
#include "ByteBuf.h"
#include "CLog.h"

class SerialDevice : public IODevice {

  using ReadCallBack = std::function<void(const char *, int)>;
 public:

  struct SerialDeviceInfo {
    std::string portName;     ///< portName 串口名称
    std::string description;  ///< description 串口描述
    std::string hardwareId;   ///< hardwareId 硬件id
  };

  enum OperateMode {
    AsynchronousOperate,  ///< Asynchronous 异步
    SynchronousOperate    ///< Synchronous 同步
  };

  enum BaudRate {
    BaudRate110 = 110,        ///< 110
    BaudRate300 = 300,        ///< 300
    BaudRate600 = 600,        ///< 600
    BaudRate1200 = 1200,      ///< 1200
    BaudRate2400 = 2400,      ///< 2400
    BaudRate4800 = 4800,      ///< 4800
    BaudRate9600 = 9600,      ///< 9600
    BaudRate14400 = 14400,    ///< 14400
    BaudRate19200 = 19200,    ///< 19200
    BaudRate38400 = 38400,    ///< 38400
    BaudRate56000 = 56000,    ///< 56000
    BaudRate57600 = 57600,    ///< 57600
    BaudRate115200 = 115200,  ///< 115200
    BaudRate921600 = 921600   ///< 921600
  };

  enum DataBits {
    DataBits5 = 5,  ///< 5 data bits 5位数据位
    DataBits6 = 6,  ///< 6 data bits 6位数据位
    DataBits7 = 7,  ///< 7 data bits 7位数据位
    DataBits8 = 8   ///< 8 data bits 8位数据位
  };

  enum Parity {
    ParityNone = 0,   ///< No Parity 无校验
    ParityOdd = 1,    ///< Odd Parity 奇校验
    ParityEven = 2,   ///< Even Parity 偶校验
    ParityMark = 3,   ///< Mark Parity 1校验
    ParitySpace = 4,  ///< Space Parity 0校验
  };

  enum StopBits {
    StopOne = 0,  ///< 1 stop bit 1位停止位
    StopTwo = 1   ///< 2 stop bit 2位停止位
  };

  enum FlowControl {
    FlowNone = 0,      ///< No flow control 无流控制
    FlowHardware = 1,  ///< Hardware(RTS / CTS) flow control 硬件流控制
    FlowSoftware = 2   ///< Software(XON / XOFF) flow control 软件流控制
  };
  
  SerialDevice(EpollLoop *loop, const std::string &name);
  ~SerialDevice() override;
   //初始化
  bool init(const std::string &portName, BaudRate baudRate =  BaudRate9600,
            Parity parity = ParityNone, DataBits dataBits = DataBits8,
            StopBits stopbits = StopOne, FlowControl flowControl = FlowNone,
            OperateMode mode = AsynchronousOperate,
            size_t readBufferSize = 4096);
            
  //Form  IODevice
  bool start() override;
  void stop() override;
  void close() override;
  void cleanup() override;
  //Send
  void send(const char *data,size_t len);
  void send(ByteBuf &buf);
  //设置读取回调
  void setReadCallback(ReadCallBack &&cb);

  void debugPrint(){
     std::cout << "12312312312312"<<std::endl;
  }
 private:
 //设置串口属性
  bool uartSet(Fd &fd, BaudRate baudRate, Parity parity, DataBits dataBits,
               StopBits stopbits, FlowControl flowControl);
  
  int baudRate2Enum(int baudrate);
  //读取处理Epoll驱动事件
  void onReadCallBack();
  //Write Event
  void onWriteCallBack();
  Fd fd_;
  EpollLoop *loop_;
  EpollFdEvent *serialEvent_;
  size_t bufferSize_;
  ByteBuf readBuffer_;
  ByteBuf writeBuffer_;
  SerialDeviceInfo info_;
  ReadCallBack readCallBack_;
  OperateMode mode_;
};
