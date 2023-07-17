#include "LogStream.h"
// Efficient Integer to String Conversions, by Matthew Wilson.
template class StreamBuffer<kSmallBuffer>;
template class StreamBuffer<kLargeBuffer>;

template <typename T>
void LogStream::formatInteger(T v) {
  if (buffer_.avail() >= kMaxNumericSize) {
    size_t len = Utils::convert(buffer_.current(), v);
    buffer_.add(len);
  }
}

LogStream &LogStream::operator<<(bool v) {
  buffer_.append(v ? "1" : "0", 1);
  return *this;
}

LogStream &LogStream::operator<<(short v) {
  *this << static_cast<int>(v);
  return *this;
}

LogStream &LogStream::operator<<(unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream &LogStream::operator<<(int v) {
  formatInteger(v);
  return *this;
}

// LogStream &LogStream::operator<<(const int v) {

//   formatInteger(v);
//   return *this;
// }


LogStream &LogStream::operator<<(unsigned int v) {
  formatInteger(v);
  return *this;
}

LogStream &LogStream::operator<<(long v) {
  formatInteger(v);
  return *this;
}

LogStream &LogStream::operator<<(unsigned long v) {
  formatInteger(v);
  return *this;
}

LogStream &LogStream::operator<<(long long v) {
  formatInteger(v);
  return *this;
}

LogStream &LogStream::operator<<(unsigned long long v) {
  formatInteger(v);
  return *this;
}

LogStream &LogStream::operator<<(const void *p) {
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  if (buffer_.avail() >= kMaxNumericSize) {
    char *buf = buffer_.current();
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = Utils::convertHex(buf + 2, v);
    buffer_.add(len + 2);
  }
  return *this;
}

LogStream &LogStream::operator<<(const char *str) {
  if (str) {
    buffer_.append(str, strlen(str));
  } else {
    buffer_.append("(null)", 6);
  }
  return *this;
}

LogStream &LogStream::operator<<(char v) {
  buffer_.append(&v, 1);
  return *this;
}

LogStream &LogStream::operator<<(float v) {
  *this << static_cast<double>(v);
  return *this;
}

LogStream &LogStream::operator<<(double v) {
  if (buffer_.avail() >= kMaxNumericSize) {
    int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
    buffer_.add(len);
  }
  return *this;
}
LogStream &LogStream::operator<<(const unsigned char *str) {
  return operator<<(reinterpret_cast<const char *>(str));
}

LogStream &LogStream::operator<<(const string &v) {
  buffer_.append(v.c_str(), v.size());
  return *this;
}
