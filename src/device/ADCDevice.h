#pragma once

#include <string>
#include "IODevice.h"

class ADCDevice : public IODevice {
 public:
  enum ADDR {
    ADS1115_ADDR_GND = 0x00, /**< ADDR pin connected to GND */
    ADS1115_ADDR_VCC = 0x01, /**< ADDR pin connected to VCC */
    ADS1115_ADDR_SDA = 0x02, /**< ADDR pin connected to SDA */
    ADS1115_ADDR_SCL = 0x03, /**< ADDR pin connected to SCL */
  };

  enum CHANNEL {
    ADS1115_CHANNEL_AIN0_AIN1 = 0x00, /**< AIN0 and AIN1 pins */
    ADS1115_CHANNEL_AIN0_AIN3 = 0x01, /**< AIN0 and AIN3 pins */
    ADS1115_CHANNEL_AIN1_AIN3 = 0x02, /**< AIN1 and AIN3 pins */
    ADS1115_CHANNEL_AIN2_AIN3 = 0x03, /**< AIN2 and AIN3 pins */
    ADS1115_CHANNEL_AIN0_GND = 0x04,  /**< AIN0 and GND pins */
    ADS1115_CHANNEL_AIN1_GND = 0x05,  /**< AIN1 and GND pins */
    ADS1115_CHANNEL_AIN2_GND = 0x06,  /**< AIN2 and GND pins */
    ADS1115_CHANNEL_AIN3_GND = 0x07,  /**< AIN3 and GND pins */
  };

  enum RANGE {
    ADS1115_RANGE_6P144V = 0x00, /**< 6.144V range */
    ADS1115_RANGE_4P096V = 0x01, /**< 4.096V range */
    ADS1115_RANGE_2P048V = 0x02, /**< 2.048V range */
    ADS1115_RANGE_1P024V = 0x03, /**< 1.024V range */
    ADS1115_RANGE_0P512V = 0x04, /**< 0.512V range */
    ADS1115_RANGE_0P256V = 0x05, /**< 0.256V range */
  };

  enum RATE {
    ADS1115_RATE_8SPS = 0x00,   /**< 8 sample per second */
    ADS1115_RATE_16SPS = 0x01,  /**< 16 sample per second */
    ADS1115_RATE_32SPS = 0x02,  /**< 32 sample per second */
    ADS1115_RATE_64SPS = 0x03,  /**< 64 sample per second */
    ADS1115_RATE_128SPS = 0x04, /**< 128 sample per second */
    ADS1115_RATE_250SPS = 0x05, /**< 250 sample per second */
    ADS1115_RATE_475SPS = 0x06, /**< 475 sample per second */
    ADS1115_RATE_860SPS = 0x07, /**< 860 sample per second */
  };

  enum COMPARE {
    ADS1115_BOOL_FALSE = 0x00, /**< disable function */
    ADS1115_BOOL_TRUE = 0x01,  /**< enable function */
  };

  ADCDevice() {}
  ~ADCDevice() override;
  bool init(const char *i2cPort, ADDR adress = ADS1115_ADDR_GND,
            CHANNEL channel = ADS1115_CHANNEL_AIN2_GND,
            RANGE range = ADS1115_RANGE_4P096V, RATE rate = ADS1115_RATE_860SPS,
            COMPARE compare = ADS1115_BOOL_TRUE);

  bool start() override { return startContinuousRead(); }
  void stop() override {}
  void close() override;
  void cleanup() override {}

  bool ads1115Read(float *s) {
    int16_t raw;
    if (continuousRead((int16_t *)&raw, s) != 0) {
      return 1;
    } else {
      return 0;
    }
  }
  //设置通道
  bool setChannel(CHANNEL channel);
  //设置采样率
  bool setRate(RATE rate);
  //设置电压范围
  bool setRange(RANGE range);
  //设置是否开启比较
  bool setCompare(COMPARE compare);

 private: 
 //开启连续读
  bool startContinuousRead();
  
  bool continuousRead(int16_t *raw, float *v);

  bool ads1115Read(uint8_t reg, int16_t *data);

  bool ads1115Write(uint8_t reg, uint16_t data);
   //I2C读
  bool iicRead(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
   //I2C写
  bool iicWrite(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

  static uint8_t convAddr(ADDR addr);

  int fd_;
  uint8_t addr_;
  CHANNEL channel_;
  RANGE range_;
  RATE rate_;
  COMPARE compare_;
};
