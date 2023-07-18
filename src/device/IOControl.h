/**
 * @file IOControl.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  GPIO控制
 * @version 0.1
 * @date 2023-07-11
 *
 *
 */
#pragma once

#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "CLog.h"

#define GPIO_U_IOCTL_BASE 'x'
#define GPIOC_OPS _IOWR(GPIO_U_IOCTL_BASE, 0, int)

class IOControl {
 public:
  enum GPIO_FUNC {
    Relay1,
    Relay2,
    Relay3,
    Relay4,
    Relay5,
    Relay6,
    Relay7,
    Relay8,
    Input1,
    Input2,
    Input3,
    Input4,
    Input5,
    Input6,
    Input7,
    Input8,
    Rs4851Ctrl,
    Rs4852Ctrl
  };

  enum PinState { GPIO_RESET, GPIO_SET };
  //设置GPIO 电平
  static void setValue(GPIO_FUNC gpio_no, PinState state) {
    unsigned long val;
    int fd = openFd();
    if (fd) {
      val = (!!(state) << 8) | (int)gpio_no;
      if (ioctl(fd, GPIOC_OPS, &val) < 0) {
        CLOG_ERROR() << "Set GPIO:" << (int)gpio_no << "ERROR";
      }
      close(fd);
    } else {
      CLOG_ERROR() << "Set GPIO:" << (int)gpio_no << "ERROR";
    }
  }
  //获取GPIO 电平
  static int getValue(GPIO_FUNC gpio_no) {
    int fd = openFd();
    unsigned long val = gpio_no;
    if (fd) {
      if (ioctl(fd, GPIOC_OPS, &val) < 0) {
        CLOG_ERROR() << "Get GPIO" << (int)gpio_no << "ERROR";
        return -1;
      }
      close(fd);
      return val;
    }
    return -1;
  }

 private:
  static int openFd() {
    int fd;
    fd = open("/dev/gpiork", O_RDWR);
    return (fd < 0) ? 0 : fd;
  }
  IOControl() {}
  ~IOControl() {}
};
