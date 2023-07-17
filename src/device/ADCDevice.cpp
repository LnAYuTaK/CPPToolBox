#include "ADCDevice.h"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "CLog.h"
#include "MacroDef.h"
#include <cassert>

#define ADS1115_ADDRESS1 (0x48 << 1) /**< iic address 1 */
#define ADS1115_ADDRESS2 (0x49 << 1) /**< iic address 2 */
#define ADS1115_ADDRESS3 (0x4A << 1) /**< iic address 3 */
#define ADS1115_ADDRESS4 (0x4B << 1) /**< iic address 4 */

#define ADS1115_REG_CONVERT 0x00  /**< adc result register */
#define ADS1115_REG_CONFIG 0x01   /**< chip config register */
#define ADS1115_REG_LOWRESH 0x02  /**< interrupt low threshold register */
#define ADS1115_REG_HIGHRESH 0x03 /**< interrupt high threshold register */

ADCDevice::ADCDevice() : type_(IODevice::ADCDeviceType), addr_(0) {}

bool ADCDevice::init(const char *i2cPort, ADDR adress, CHANNEL channel,
                     RANGE range, RATE rate ,COMPARE compare) 
{
  addr_ = convAddr(adress);
  channel_ = channel;
  range_ = range;
  rate_ = rate;
  compare_ = compare;
  fd_ = ::open(i2cPort,O_RDWR);
  /* check the fd */
  if ((fd_) < 0)
  {
      CLOG_ERROR() << "Open ADC Device Error"; 
      return false;
  }
  setChannel(channel_);
  setRate(rate_);
  setCompare(compare_);
  setRange(range_);
  return true;
}

void ADCDevice::close() {
  int fd = fd_;
  CHECK_CLOSE_RESET_FD(fd);
}

uint8_t ADCDevice::convAddr(ADDR adress) {
  switch (adress) {
    case ADS1115_ADDR_GND:
      return ADS1115_ADDRESS1;
    case ADS1115_ADDR_VCC:
      return ADS1115_ADDRESS2;
    case ADS1115_ADDR_SDA:
      return ADS1115_ADDRESS3;
    case ADS1115_ADDR_SCL:
      return ADS1115_ADDRESS4;
    default:
      break;
  }
  return ADS1115_ADDRESS1;
}

bool ADCDevice::setChannel(CHANNEL channel)
{
    bool res;
    uint16_t conf;
    res = ads1115Read(ADS1115_REG_CONFIG, (int16_t *)&conf);       
    if (!res)                                                                       
    {
      CLOG_ERROR()<<  "ADC Read Reg Config Error";                    
      return false;                                                                        
    }
    conf &= ~(0x07 << 12);                                                                
    conf |= (channel & 0x07) << 12;                                                      
    res = ads1115Write(ADS1115_REG_CONFIG, conf);                  
    if (!res)                                                                        
    {
      CLOG_ERROR()<< "ADC Write Reg Config Error";                           
      return false;                                                                        
    }
    return 0;   
}

bool ADCDevice::setRate(RATE rate)
{
    bool res;
    uint16_t conf;
    res = ads1115Read(ADS1115_REG_CONFIG, (int16_t *)&conf); 
    if (!res)                                                                        
    {
        CLOG_ERROR() << "ADC Read Reg Config Error";
        return false;                                                                        
    }
    conf &= ~(0x07 << 5);                                                                
    conf |= (rate & 0x07) << 5;     
                                               
    res = ads1115Write(ADS1115_REG_CONFIG, conf);                  
    if (!res)                                                                        
    {
        CLOG_ERROR() << "ADC Write Reg Config Error";
        return false;                                                                          
    }
    return true;    
}

bool ADCDevice::setRange(RANGE range)
{
    bool res;
    uint16_t conf;
    res = ads1115Read(ADS1115_REG_CONFIG, (int16_t *)&conf);     
    if (!res)                                                                          
    {
      CLOG_ERROR()<<"ADC Read Reg Config Error";
      return false;                                                                       
    }
    conf &= ~(0x07 << 9);                                                                
    conf |= (range & 0x07) << 9;                                                         
    res = ads1115Write(ADS1115_REG_CONFIG, conf);                  
    if (!res)                                                                         
    {
      CLOG_ERROR()<<"ADC Write Reg Config Error";
      return false;                                                                       
    }
    return true;   
}

bool ADCDevice::setCompare(COMPARE compare)
{
    bool res;
    uint16_t conf;
    res = ads1115Read(ADS1115_REG_CONFIG, (int16_t *)&conf);     
    if (!res)                                                                      
    {
      CLOG_ERROR()<<"ADC Read Reg Config Error";
      return false;                                                                        
    }
    conf &= ~(0x01 << 2);                                                              
    conf |= compare << 2;                                                                   
    res = ads1115Write(ADS1115_REG_CONFIG, conf);               
    if (!res)                                                                       
    {
      CLOG_ERROR()<<"ADC Write Reg Config Error";
      return false;                                                                         
    }
    return true;  
}

bool ADCDevice::startContinuousRead()
{
    bool  res;
    uint16_t conf;
    res = ads1115Read(ADS1115_REG_CONFIG, (int16_t *)&conf);       
    if (!res)                                                                          
    {
      CLOG_ERROR()<<"ADC Read Reg Config Error";
      return false;                                                                         
    }
    conf &= ~(0x01 << 8);                                                                
    res = ads1115Write(ADS1115_REG_CONFIG, conf);                  
    if (!res)                                                                         
    {
      CLOG_ERROR()<<"ADC Write Reg Config Error";
      return false;                                                                     
    }
    return true;  
}

bool ADCDevice::continuousRead(int16_t *raw, float *v)
{
    bool res;
    uint8_t range;
    uint16_t conf;
    res = ads1115Read(ADS1115_REG_CONFIG, (int16_t *)&conf);      
    if (!res)                                                                       
    {
        return false;                                                                        
    }
    range = (RANGE)((conf >> 9) & 0x07);                                      
    res = ads1115Read(ADS1115_REG_CONVERT,raw);                 
    if (!res)                                                                        
    {    
        return false;                                                                         
    }
    if (range == ADS1115_RANGE_6P144V)                                                    
    {
        *v = (float)(*raw) * 6.144f / 32768.0f;                                          
    }
    else if (range == ADS1115_RANGE_4P096V)                                               
    {
        *v = (float)(*raw) * 4.096f / 32768.0f;                                            
    }
    else if (range == ADS1115_RANGE_2P048V)                                                
    {
        *v = (float)(*raw) * 2.048f / 32768.0f;                                           
    }
    else if (range == ADS1115_RANGE_1P024V)                                              
    {
        *v = (float)(*raw) * 1.024f / 32768.0f;                                           
    }
    else if (range == ADS1115_RANGE_0P512V)                                             
    {
        *v = (float)(*raw) * 0.512f / 32768.0f;                                       
    }
    else if (range == ADS1115_RANGE_0P256V)                                              
    {
        *v = (float)(*raw) * 0.256f / 32768.0f;                                          
    }
    else
    {
        return false;                                                                       
    }
    return true;                                                                             
}

bool ADCDevice::ads1115Read(uint8_t reg, int16_t *data) {
  uint8_t buf[2];
  memset(buf, 0, sizeof(uint8_t) * 2);        /* clear the buffer */
  if (iicRead(addr_, reg, (uint8_t *)buf, 2)) /* read data */
  {
    *data = (uint16_t)(((uint16_t)buf[0] << 8) | buf[1]); /* set data */
    return true;                                          /* success return 0 */
  } else {
    return false; /* return error */
  }
}

bool ADCDevice::ads1115Write(uint8_t reg, uint16_t data) {
  uint8_t buf[2];

  buf[0] = (data >> 8) & 0xFF;                 /* set MSB */
  buf[1] = data & 0xFF;                        /* set LSB */
  if (iicWrite(addr_, reg, (uint8_t *)buf, 2)) /* write data */
  {
    return true; /* return error */
  } else {
    return false; /* success return 0 */
  }
}

bool ADCDevice::iicRead(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len) {
  int fd = fd_;
  struct i2c_rdwr_ioctl_data i2c_rdwr_data;
  struct i2c_msg msgs[2];

  memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));

  memset(msgs, 0, sizeof(struct i2c_msg) * 2);

  msgs[0].addr = addr >> 1;
  msgs[0].flags = 0;
  msgs[0].buf = &reg;
  msgs[0].len = 1;
  msgs[1].addr = addr >> 1;
  msgs[1].flags = I2C_M_RD;
  msgs[1].buf = buf;
  msgs[1].len = len;
  i2c_rdwr_data.msgs = msgs;
  i2c_rdwr_data.nmsgs = 2;
  if (ioctl(fd, I2C_RDWR, &i2c_rdwr_data) < 0) {
    CLOG_ERROR() << "ERROR READ I2C!!!!!!!!!!!" ;
    return false;
  }
  return true;
}

bool ADCDevice::iicWrite(uint8_t addr, uint8_t reg, uint8_t *buf,
                         uint16_t len) {
  int fd = fd_;
  struct i2c_rdwr_ioctl_data i2c_rdwr_data;
  struct i2c_msg msgs[1];
  uint8_t buf_send[len + 1];

  /* clear ioctl data */
  memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));

  /* clear msgs data */
  memset(msgs, 0, sizeof(struct i2c_msg) * 1);

  /* clear sent buf */
  memset(buf_send, 0, sizeof(uint8_t) * (len + 1));

  /* set the param */
  msgs[0].addr = addr >> 1;
  msgs[0].flags = 0;
  buf_send[0] = reg;
  memcpy(&buf_send[1], buf, len);
  msgs[0].buf = buf_send;
  msgs[0].len = len + 1;
  i2c_rdwr_data.msgs = msgs;
  i2c_rdwr_data.nmsgs = 1;
  /* transmit */
  if (ioctl(fd, I2C_RDWR, &i2c_rdwr_data) < 0) {
    CLOG_WARN() << "I2C Write ERROR";
    return false;
  }
  return true;
}

ADCDevice::~ADCDevice() {}
