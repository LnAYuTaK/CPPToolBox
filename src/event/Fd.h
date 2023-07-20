/**
 * @file Fd.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  文件描述符类，封装了对fd的基础操作
 * @version 0.1
 * @date 2023-07-11
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <sys/uio.h>
#include <unistd.h>
#include <functional>
#include "CLog.h"

class Fd {
 public:
  using CloseFunc = std::function<void(int)>;

  Fd();
  Fd(int fd);
  Fd(int fd, const CloseFunc &close_func);
  virtual ~Fd();

  Fd(const Fd &other);
  Fd &operator=(const Fd &other);

  Fd(Fd &&other);
  Fd &operator=(Fd &&other);
  /**
   * @brief 重置Fd
   * 
   */
  void reset();  
  /**
   * @brief 交换Fd
   * 
   * @param other 
   */
  void swap(Fd &other);

  /**
   * @brief 提前关闭文件描述符
   * 不管是否有其他引用！
   * 
   */
  void close();
  /**
   * @brief 判断fd资源是否为空
   * 
   * @return true 
   * @return false 
   */
  inline bool isNull() const { return detail_ == nullptr || detail_->fd == -1; }

 public:  
 /**
  * @brief 初始化创建Fd
  * @param filename 
  * @param flags 
  * @return Fd 
  */
  static Fd Open(const char *filename, int flags);

 public:
  /**
   * @brief 获取到原始fd
   * 
   * @return int 
   */
  inline int get() const { return (detail_ != nullptr) ? detail_->fd : -1; }

  /**
   * @brief 读取
   * 
   * @param ptr 
   * @param size 
   * @return ssize_t 
   */
  ssize_t read(void *ptr, size_t size) const;
  ssize_t readv(const struct iovec *iov, int iovcnt) const;

    /**
   * @brief 写入
   * 
   * @param ptr 
   * @param size 
   * @return ssize_t 
   */
  ssize_t write(const void *ptr, size_t size) const;
  ssize_t writev(const struct iovec *iov, int iovcnt) const;

  /**
   * @brief 开启或关闭非阻塞选项
   * 
   * @param enable 
   */
  void setNonBlock(bool enable) const; 
  /**
   * @brief 检查是否非阻塞
   * 
   * @return true 
   * @return false 
   */
  bool isNonBlock() const;    
  /**
   * @brief 设置Exec时关闭选项
   * 
   * @return * void 
   */
  void setCloseOnExec() const; 

 protected:
  struct Detail {
    int fd = -1;
    int ref_count = 1;
    CloseFunc close_func;
  };
 private:
  Detail *detail_ = nullptr;
};
