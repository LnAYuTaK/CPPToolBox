#pragma once

#include <sys/uio.h>
#include <unistd.h>
#include <functional>
#include "CLog.h"

//! 文件描述符类，封装了对fd的基础操作
class Fd {
 public:
  using CloseFunc = std::function<void(int)>;

  Fd();
  Fd(int fd);
  Fd(int fd, const CloseFunc &close_func);
  virtual ~Fd();

  Fd(const Fd &other);

  void swap(Fd &other);

  //! 提前关闭资源，无论是否还有其它Fd对象引用
  void close(); 

  inline bool isNull() const { return detail_ == nullptr || detail_->fd == -1; }

 public:  //! 创建函数
  static Fd Open(const char *filename, int flags);

 public:
  //! 获取文件描述符的值。注意谨慎使用
  inline int get() const { return (detail_ != nullptr) ? detail_->fd : -1; }

  //! 读
  ssize_t read(void *ptr, size_t size) const;
  ssize_t readv(const struct iovec *iov, int iovcnt) const;

  //! 写
  ssize_t write(const void *ptr, size_t size) const;
  ssize_t writev(const struct iovec *iov, int iovcnt) const;

  //! 其它
  void setNonBlock(bool enable) const;  //! 开启或关闭非阻塞选项
  bool isNonBlock() const;              //! 检查是否非阻塞
  void setCloseOnExec() const;          //! 设置Exec时关闭选项

 protected:
  struct Detail {
    int fd = -1;
    int ref_count = 1;
    CloseFunc close_func;
  };

 private:
  Detail *detail_ = nullptr;
};
