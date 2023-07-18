#include "SerialDevice.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include "Bytes.h"
#include "CLog.h"
#include "MacroDef.h"

#include <fcntl.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <termios.h>

SerialDevice::SerialDevice(EpollLoop *loop, const std::string &name)
    : loop_(loop),
      fd_(-1),
      serialEvent_(nullptr),
      readBuffer_(nullptr),
      writeBuffer_(nullptr) {
      name_ = name;
}
SerialDevice::~SerialDevice() { cleanup(); }

bool SerialDevice::init(const std::string &portName, BaudRate baudRate,
                        Parity parity, DataBits dataBits, StopBits stopbits,
                        FlowControl flowControl, OperateMode mode,
                        size_t bufferSize) {
CLOG_DEBUG()  << ": DEBUG3";
  if(state()  != State::kNone){
      return false;
  }
  mode_ = mode;
  CHECK_DELETE_RESET_OBJ(readBuffer_);
  CHECK_DELETE_RESET_OBJ(writeBuffer_);
CLOG_DEBUG()  << ": DEBUG4";
  readBuffer_ = new Bytes(bufferSize_);
  writeBuffer_ = new Bytes(bufferSize_);
  serialEvent_ = loop_->creatFdEvent(name_);
   CLOG_DEBUG()  << ": DEBUG5";
  fd_ = Fd::Open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  CLOG_DEBUG()  << ": DEBUG6";
  if (!(serialEvent_->init(fd_.get(), Event::Mode::kPersist))) {
    CLOG_ERROR() <<  name_ << ": Serial Event Init Error";
    return false;
  }
     CLOG_DEBUG()  << ": DEBUG7";
  if (!(uartSet(fd_, baudRate, parity, dataBits, stopbits, flowControl))) {
    CLOG_ERROR() <<   name_ << ":  " << portName << " Serial Set Error";
    return false;
  }
  setState(State::kInited);
  return true;
}

bool SerialDevice::start() {
  if(state()  == State::kInited ) {
      serialEvent_->enableReading();
      setState(State::kRunning);
      return true;
  }
  else {
    if(state() == State::kNone) {
          CLOG_WARN() << name() << ":  Serial  Need Init";
    }
     return false;
  }
}

void SerialDevice::stop() {
  if(state()  == State::kRunning) {
       serialEvent_-> disableAll();
  }
  else {
    CLOG_WARN() << name() << ":  Serial  Need Init";
  }
}

void SerialDevice::close() {
          stop();
          cleanup();
          setState(State::kNone);
}

void SerialDevice::cleanup() {

    CHECK_DELETE_RESET_OBJ(writeBuffer_);
    CHECK_DELETE_RESET_OBJ(readBuffer_);
    CHECK_DELETE_RESET_OBJ(serialEvent_);
}

void SerialDevice::setReadCallback(ReadCallBack &&cb) {
  readCallBack_ = std::move(cb);
  if (readCallBack_) {
    if (serialEvent_) {
          serialEvent_->setReadCallback([&](int time) {
        { this->onReadCallBack(); }
      });
    }
  } else {
    CLOG_ERROR() << "Serial Need init First";
  }
}

bool SerialDevice::uartSet(Fd &fd, BaudRate baudRate, Parity parity,
                           DataBits dataBits, StopBits stopbits,
                           FlowControl flowControl) {
  struct termios options;
  // 获取终端属性

  if (tcgetattr(fd.get(), &options) < 0) {
    CLOG_ERROR() << "Need Open First";
    return false;
  }

  int baudRateConstant = baudRate2Enum((int)baudRate);
  if (baudRateConstant != 0) {
    cfsetispeed(&options, baudRateConstant);
    cfsetospeed(&options, baudRateConstant);
  }
  // 设置校验位
  switch (parity) {
    // 无奇偶校验位
    case ParityNone:
      options.c_cflag &= ~PARENB;  // PARENB：产生奇偶位，执行奇偶校验
      options.c_cflag &= ~INPCK;  // INPCK：使奇偶校验起作用
      break;
    // 设置奇校验
    case ParityOdd:
      options.c_cflag |= PARENB;  // PARENB：产生奇偶位，执行奇偶校验
      options.c_cflag |= PARODD;  // PARODD：若设置则为奇校验,否则为偶校验
      options.c_cflag |= INPCK;  // INPCK：使奇偶校验起作用
      options.c_cflag |= ISTRIP;  // ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
      break;
    // 设置偶校验
    case ParityEven:
      options.c_cflag |= PARENB;  // PARENB：产生奇偶位，执行奇偶校验
      options.c_cflag &= ~PARODD;  // PARODD：若设置则为奇校验,否则为偶校验
      options.c_cflag |= INPCK;  // INPCK：使奇偶校验起作用
      options.c_cflag |=ISTRIP;  // ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
      break;
    // 设置0校验
    case ParitySpace:
      options.c_cflag &= ~PARENB;  // PARENB：产生奇偶位，执行奇偶校验
      options.c_cflag &= ~CSTOPB;  // CSTOPB：使用两位停止位
      break;
    default:
      CLOG_ERROR() << "unknown Serial  Party";
      return false;
  }

  // 设置数据位
  switch (dataBits) {
    case DataBits5:
      options.c_cflag &= ~CSIZE;  // 屏蔽其它标志位
      options.c_cflag |= CS5;
      break;
    case DataBits6:
      options.c_cflag &= ~CSIZE;  // 屏蔽其它标志位
      options.c_cflag |= CS6;
      break;
    case DataBits7:
      options.c_cflag &= ~CSIZE;  // 屏蔽其它标志位
      options.c_cflag |= CS7;
      break;
    case DataBits8:
      options.c_cflag &= ~CSIZE;  // 屏蔽其它标志位
      options.c_cflag |= CS8;
      break;
    default:
      CLOG_ERROR() << "Unknow Serial DataBits";
      return false;
  }

  // 停止位
  switch (stopbits) {
    case StopOne:
      options.c_cflag &= ~CSTOPB;  // CSTOPB：使用1位停止位
      break;
    case StopTwo:
      options.c_cflag |= CSTOPB;  // CSTOPB：使用2位停止位
      break;
    default:
      CLOG_ERROR() << "Unknow Serial StopBits";
      return false;
  }
  // 流控制
  switch (flowControl) {
    case FlowNone:  ///< No flow control 无流控制
      options.c_cflag &= ~CRTSCTS;
      break;
    case FlowHardware:  ///< Hardware(RTS / CTS) flow control 硬件流控制
      options.c_cflag |= CRTSCTS;
      break;
    case FlowSoftware:  ///< Software(XON / XOFF) flow control 软件流控制
      options.c_cflag |= IXON | IXOFF | IXANY;
      break;
    default:
      CLOG_ERROR() << "Unknow Serial FlowControl";
      return false;
  }

  options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IUCLC | IXON | IXANY | IXOFF);
  options.c_iflag |= IGNBRK | IGNPAR;
  options.c_oflag &= ~(OPOST | OLCUC | ONLCR | OCRNL | ONOCR | ONLRET);
  options.c_cflag &= ~(CRTSCTS);
  options.c_cflag |= CREAD | HUPCL | CLOCAL;
  options.c_lflag &= ~(ISIG | ICANON | ECHO | IEXTEN);
  tcflush(fd.get(), TCIFLUSH);
  //激活配置
  if (tcsetattr(fd.get(), TCSANOW, &options) < 0) {
    CLOG_ERROR() << " Tcsetattr  Serial Error";
    return false;
  }
  return true;
}

int SerialDevice::baudRate2Enum(int baudrate) {
  switch (baudrate) {
#ifdef B50
    case (50):
      return B50;
#endif /* B50 */
#ifdef B75
    case (75):
      return B75;
#endif /* B75 */
#ifdef B110
    case (110):
      return B110;
#endif /* B110 */
#ifdef B134
    case (134):
      return B134;
#endif /* B134 */
#ifdef B150
    case (150):
      return B150;
#endif /* B150 */
#ifdef B200
    case (200):
      return B200;
#endif /* B200 */
#ifdef B300
    case (300):
      return B300;
#endif /* B300 */
#ifdef B600
    case (600):
      return B600;
#endif /* B600 */
#ifdef B1200
    case (1200):
      return B1200;
#endif /* B1200 */
#ifdef B1800
    case (1800):
      return B1800;
#endif /* B1800 */
#ifdef B2400
    case (2400):
      return B2400;
#endif /* B2400 */
#ifdef B4800
    case (4800):
      return B4800;
#endif /* B4800 */
#ifdef B9600
    case (9600):
      return B9600;
#endif /* B9600 */
#ifdef B19200
    case (19200):
      return B19200;
#endif /* B19200 */
#ifdef B38400
    case (38400):
      return B38400;
#endif /* B38400 */
#ifdef B57600
    case (57600):
      return B57600;
#endif /* B57600 */
#ifdef B115200
    case (115200):
      return B115200;
#endif /* B115200 */
#ifdef B230400
    case (230400):
      return B230400;
#endif /* B230400 */
#ifdef B345600
    case (345600):
      return B345600;
#endif /* B345600 */
#ifdef B460800
    case (460800):
      return B460800;
#endif /* B460800 */
#ifdef B500000
    case (500000):
      return B500000;
#endif /* B500000 */
#ifdef B576000
    case (576000):
      return B576000;
#endif /* B576000 */
#ifdef B921600
    case (921600):
      return B921600;
#endif /* B921600 */
#ifdef B1000000
    case (1000000):
      return B1000000;
#endif /* B1000000 */
#ifdef B1152000
    case (1152000):
      return B1152000;
#endif /* B1152000 */
#ifdef B1500000
    case (1500000):
      return B1500000;
#endif /* B1500000 */
#ifdef B2000000
    case (2000000):
      return B2000000;
#endif /* B2000000 */
#ifdef B2500000
    case (2500000):
      return B2500000;
#endif /* B2500000 */
#ifdef B3000000
    case (3000000):
      return B3000000;
#endif /* B3000000 */
#ifdef B3500000
    case (3500000):
      return B3500000;
#endif /* B3500000 */
#ifdef B4000000
    case (4000000):
      return B4000000;
#endif /* B4000000 */
    default:
      return -1;
  }
}

void SerialDevice::onReadCallBack()
{
  if(readBuffer_  ==  nullptr)
  {
    return ;
  }
  //双缓存读取
  struct iovec rbuf[2];
  char bufferTemp[1024];
  char bufferTemp2[1024];
  bzero(bufferTemp,sizeof(bufferTemp));
  bzero(bufferTemp2,sizeof(bufferTemp2));

  rbuf[0].iov_base = bufferTemp;
  rbuf[0].iov_len  = sizeof(bufferTemp);
  rbuf[1].iov_base = bufferTemp2;
  rbuf[1].iov_len  = sizeof(bufferTemp2);

  ssize_t rsize = fd_.readv(rbuf, 2);
  if(rsize> 0 ) {    //说明读取到了数据
      do {
            //如果一次读取超过了缓存区1 那么数据应该在缓存区2有一部分
            if (static_cast<size_t>(rsize) > sizeof(bufferTemp))
            {
                 //那么超过的数据应该是
                size_t reMainData =  rsize -sizeof(bufferTemp); 
                readBuffer_->append(bufferTemp,sizeof(bufferTemp));
                readBuffer_->append(bufferTemp2,reMainData);
            }
            else
            {
                 readBuffer_->append(bufferTemp,rsize);   
            }
      } while (rsize > 0);
  }
  CLOG_INFO()<< readBuffer_->ptr();
}