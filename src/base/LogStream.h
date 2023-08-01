/**
 * @file LogStream.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief 日志流
 * @version 0.1
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <assert.h>
#include <iostream>
#include <string>
// common
#include "MacroDef.h"
#include "Utils.h"

using namespace std;
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class StreamBuffer {
 public:
  StreamBuffer() : cur_(data_) {}
  ~StreamBuffer() {}
  void append(const char* /*restrict*/ buf, size_t len) {
    if (Utils::implicit_cast<size_t>(avail()) > len) {
      memcpy(cur_, buf, len);
      cur_ += len;
    }
  }
  const char* data() const { return data_; }
  int length() const { return static_cast<int>(cur_ - data_); }
  char* current() { return cur_; }
  int avail() const { return static_cast<int>(end() - cur_); }
  void add(size_t len) { cur_ += len; }
  void reset() { cur_ = data_; }
  void bzero() { Utils::memZero(data_, sizeof data_); }
  string toString() const { return string(data_, length()); }

 private:
  const char* end() const { return data_ + sizeof data_; }
  void (*cookie_)();
  char data_[SIZE];
  char* cur_;
};

class LogStream {
  using Self = LogStream;
  using Buffer = StreamBuffer<kSmallBuffer>;

 public:
  Self& operator<<(bool v);
  Self& operator<<(short);
  Self& operator<<(unsigned short);
  Self& operator<<(int);
  // Self& operator<<(const int v);
  Self& operator<<(unsigned int);
  Self& operator<<(long);
  Self& operator<<(unsigned long);
  Self& operator<<(long long);
  Self& operator<<(unsigned long long);
  Self& operator<<(const void*);
  Self& operator<<(float v);
  Self& operator<<(double);
  Self& operator<<(char v);
  Self& operator<<(const char* str);
  Self& operator<<(const unsigned char* str);
  Self& operator<<(const string& v);

  void append(const char* data, int len) { buffer_.append(data, len); }
  const Buffer& buffer() const { return buffer_; }
  void zeroBuffer() { buffer_.bzero(); }
  void resetBuffer() { buffer_.reset(); }

 private:
  template <typename T>
  void formatInteger(T);
  Buffer buffer_;
  static const int kMaxNumericSize = 48;
};