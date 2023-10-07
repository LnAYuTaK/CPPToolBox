#include "ByteBuf.h"
using namespace gwcpp;
  namespace {
  #define INT8_BIT sizeof(int8_t)
  #define INT16_BIT sizeof(int16_t)
  #define INT32_BIT sizeof(int32_t)
  #define INT64_BIT sizeof(int64_t)
  #define FLOAT_BIT sizeof(float)
  #define DOUBLE_BIT sizeof(double)
  }  // namespace

ByteBuf::ByteBuf() { d = std::unique_ptr<::ByteBufData>(new ByteBufData()); }
ByteBuf::ByteBuf(const char *data, int32_t size) {
  d = std::unique_ptr<::ByteBufData>(new ByteBufData());
  d->originData.append(data, size);
  d->writerIndex += size;
}
ByteBuf::ByteBuf(const std::string &data) {
  d = std::unique_ptr<::ByteBufData>(new ByteBufData());
  d->originData.append(data);
  d->writerIndex += data.length();
}
ByteBuf::ByteBuf(int32_t size, char ch) {
  d = std::unique_ptr<::ByteBufData>(new ByteBufData());
  d->originData.append(size, ch);
  d->writerIndex += size;
}
ByteBuf::ByteBuf(const ByteBuf &other) {
  d = std::unique_ptr<::ByteBufData>(new ByteBufData());
  d->originData.append(other.d->originData);
  d->readerIndex = other.d->readerIndex;
  d->writerIndex = other.d->writerIndex;
  d->markedReaderIndex = other.d->markedReaderIndex;
  d->markedWriterIndex = other.d->markedWriterIndex;
}
ByteBuf::~ByteBuf() = default;

ByteBuf &ByteBuf::operator=(const ByteBuf &other) {
  if (&other != this) {
    d->originData.append(other.d->originData);
    d->readerIndex = other.d->readerIndex;
    d->writerIndex = other.d->writerIndex;
    d->markedReaderIndex = other.d->markedReaderIndex;
    d->markedWriterIndex = other.d->markedWriterIndex;
  }
  return *this;
}

std::string ByteBuf::toHexString(Case type, const std::string &fill_str) const {
  std::string result;
  for (char ch : d->originData) {
    if (Case::Lower == type) {
      result.push_back(toHexLower(ch >> 4));
      result.push_back(toHexLower(ch & 0xF));
    } else {
      result.push_back(toHexUpper(ch >> 4));
      result.push_back(toHexUpper(ch & 0xF));
    }
    result.append(fill_str);
  }
  return result;
}

const char *ByteBuf::data() const noexcept { return d->originData.data(); }
int32_t ByteBuf::bytesBefore(char value) {
  for (int32_t index = d->readerIndex; index < d->writerIndex; index++) {
    if (d->originData.at(index) == value) {
      return index - d->readerIndex;
    }
  }
  return -1;
}
int32_t ByteBuf::bytesBefore(int32_t length, char value) {
  return bytesBefore(d->readerIndex, length, value);
}
int32_t ByteBuf::bytesBefore(int32_t index, int32_t length, char value) {
  bool last_ok;
  int32_t second_index;
  for (; index < d->writerIndex - length; index++) {
    //找到第一个符合 && 剩余的长度足够
    if (d->originData.at(index) == value && (index + length) < d->writerIndex) {
      last_ok = true;
      //从后一个开始检查length-1个字符
      for (second_index = index + 1; second_index < (index + length);
           second_index++) {
        if (d->originData.at(second_index) != value) {
          last_ok = false;
          break;
        }
      }
      if (last_ok) {
        return index - d->readerIndex;
      }
    }
  }
  return -1;
}
int32_t ByteBuf::capacity() const { return (int32_t)d->originData.capacity(); }
ByteBuf &ByteBuf::capacity(int32_t new_capacity) {
  d->originData.reserve(new_capacity);
  return *this;
}
ByteBuf &ByteBuf::clear() {
  d->readerIndex = 0;
  d->writerIndex = 0;
  return *this;
}
int32_t ByteBuf::compareTo(const ByteBuf &buffer) const {
  return d->originData.compare(buffer.d->originData);
}
ByteBuf ByteBuf::copy() const { return ByteBuf(d->originData); }
ByteBuf ByteBuf::copy(int32_t index, int32_t length) const {
  return ByteBuf(d->originData.substr(index, length));
}
ByteBuf &ByteBuf::discardReadBytes() {
  d->originData = d->originData.substr(d->readerIndex + 1);
  d->readerIndex = 0;
  d->writerIndex = 0;
  return *this;
}

int32_t ByteBuf::indexOf(int32_t fromIndex, int32_t toIndex, char value) const {
  for (int32_t index = fromIndex;
       index < d->originData.length() && index < toIndex; index++) {
    if (d->originData.at(index) == value) {
      return index;
    }
  }
  return -1;
}
ByteBuf ByteBuf::setIndex(int32_t readerIndex, int32_t writerIndex) {
  if (d->originData.empty()) {
    return *this;
  }
  d->readerIndex = readerIndex;
  d->writerIndex = writerIndex;
  if (d->readerIndex >= d->originData.length()) {
    d->readerIndex = (int32_t)d->originData.length() - 1;
  }
  if (d->writerIndex > d->originData.length()) {
    d->writerIndex = (int32_t)d->originData.length();
  }
  return *this;
}
bool ByteBuf::isReadable() const {
  return (d->writerIndex - d->readerIndex) > 0;
}
bool ByteBuf::isReadable(int32_t size) {
  return (d->writerIndex - d->readerIndex) >= size;
}
ByteBuf &ByteBuf::markReaderIndex() {
  d->markedReaderIndex = d->readerIndex;
  return *this;
}
ByteBuf &ByteBuf::markWriterIndex() {
  d->markedWriterIndex = d->writerIndex;
  return *this;
}
int32_t ByteBuf::readerIndex() const { return d->readerIndex; }

ByteBuf &ByteBuf::readerIndex(int32_t reader_index) {
  if (reader_index <= d->writerIndex) {
    d->readerIndex = reader_index;
  }
  return *this;
}
ByteBuf &ByteBuf::resetReaderIndex() {
  d->readerIndex = d->markedReaderIndex;
  return *this;
}
int32_t ByteBuf::writerIndex() const { return d->writerIndex; }
ByteBuf &ByteBuf::writerIndex(int32_t writer_index) {
  if (writer_index < d->originData.length()) {
    d->writerIndex = writer_index;
  }
  return *this;
}
ByteBuf &ByteBuf::resetWriterIndex() {
  d->writerIndex = d->markedWriterIndex;
  return *this;
}
int32_t ByteBuf::readableBytes() const {
  return d->writerIndex - d->readerIndex;
}
char ByteBuf::getByte(int32_t index) const {
  if (index >= d->originData.length()) {
    return 0;
  }
  return d->originData.at(index);
}
int32_t ByteBuf::getBytes(int32_t index, char *dst) const {
  if (index >= d->originData.length()) {
    return 0;
  }
  std::string res = d->originData.substr(index);
  *dst = *res.data();
  return (int32_t)res.length();
}
int32_t ByteBuf::getBytes(int32_t index, char *dst, int32_t length) const {
  if (index >= d->originData.length()) {
    return 0;
  }
  std::string res = d->originData.substr(index, length);
  *dst = *res.data();
  return (int32_t)res.length();
}
ByteBuf ByteBuf::getBytes(int32_t index) const {
  if (index >= d->originData.length()) {
    return ByteBuf();
  }
  std::string res = d->originData.substr(index);
  return ByteBuf(res);
}
ByteBuf ByteBuf::getBytes(int32_t index, int32_t length) const {
  if (index >= d->originData.length()) {
    return ByteBuf();
  }
  std::string res = d->originData.substr(index, length);
  return ByteBuf(res);
}
ByteBuf ByteBuf::getBytes(int32_t index, int32_t dstIndex,
                          int32_t length) const {
  if (index >= d->originData.length()) {
    return ByteBuf();
  }
  ByteBuf data(dstIndex, 0);
  std::string res = d->originData.substr(index, length);
  data.writeBytes(res.data(), (int32_t)res.length());
  return data;
}
int8_t ByteBuf::getChar(int32_t index) const {
  if (index >= d->originData.length()) {
    return 0;
  }
  return d->originData.at(index);
}
uint8_t ByteBuf::getUnsignedChar(int32_t index) const { return getChar(index); }
int16_t ByteBuf::getShort(int32_t index) const {
  return d->getT<int16_t>(index, INT16_BIT);
}
int16_t ByteBuf::getShortLE(int32_t index) const {
  return d->getTLE<int16_t>(index, INT16_BIT);
}
uint16_t ByteBuf::getUnsignedShort(int32_t index) const {
  return d->getT<int16_t>(index, INT16_BIT);
}
uint16_t ByteBuf::getUnsignedShortLE(int32_t index) const {
  return d->getTLE<int16_t>(index, INT16_BIT);
}
int32_t ByteBuf::getInt(int32_t index) const {
  return d->getT<int32_t>(index, INT32_BIT);
}
int32_t ByteBuf::getIntLE(int32_t index) const {
  return d->getTLE<int32_t>(index, INT32_BIT);
}
uint32_t ByteBuf::getUnsignedInt(int32_t index) const {
  return d->getT<int32_t>(index, INT32_BIT);
}
uint32_t ByteBuf::getUnsignedIntLE(int32_t index) const {
  return d->getTLE<int32_t>(index, INT32_BIT);
}
int64_t ByteBuf::getLong(int32_t index) const {
  return d->getT<int64_t>(index, INT64_BIT);
}
int64_t ByteBuf::getLongLE(int32_t index) const {
  return d->getTLE<int64_t>(index, INT64_BIT);
}
uint64_t ByteBuf::getUnsignedLong(int32_t index) const {
  return d->getT<int64_t>(index, INT64_BIT);
}
uint64_t ByteBuf::getUnsignedLongLE(int32_t index) const {
  return d->getTLE<int64_t>(index, INT64_BIT);
}
float ByteBuf::getFloat(int32_t index) const {
  float result = 0.00;
  if (d->originData.length() - index < FLOAT_BIT) {
    return 0.00;
  }
  char *p = (char *)&result;
  for (; index < index + FLOAT_BIT; index++) {
    *(p + index) = d->originData.at(index);
  }
  return result;
}
float ByteBuf::getFloatLE(int32_t index) const {
  float result = 0.00;
  if (d->originData.length() - index < FLOAT_BIT) {
    return 0.00;
  }
  char *p = (char *)&result;
  for (; index < index + FLOAT_BIT; index++) {
    *(p + index) = d->originData.at(FLOAT_BIT - index - 1);
  }
  return result;
}

double ByteBuf::getDouble(int32_t index) const {
  double result = 0.00;
  if (d->originData.length() - index < DOUBLE_BIT) {
    return 0.00;
  }
  char *p = (char *)&result;
  for (; index < index + DOUBLE_BIT; index++) {
    *(p + index) = d->originData.at(index);
  }
  return result;
}
double ByteBuf::getDoubleLE(int32_t index) const {
  double result = 0.00;
  if (d->originData.length() - index < DOUBLE_BIT) {
    return 0.00;
  }
  char *p = (char *)&result;
  for (; index < index + DOUBLE_BIT; index++) {
    *(p + index) = d->originData.at(DOUBLE_BIT - index - 1);
  }
  return result;
}
int8_t ByteBuf::readChar() {
  if (d->originData.length() - d->readerIndex < INT8_BIT) {
    return 0;
  }
  return d->originData.at(d->readerIndex++);
}
uint8_t ByteBuf::readUnsignedChar() { return readChar(); }
int16_t ByteBuf::readShort() { return d->readT<int16_t>(INT16_BIT); }
int16_t ByteBuf::readShortLE() { return d->readTLE<int16_t>(INT16_BIT); }
uint16_t ByteBuf::readUnsignedShort() { return d->readT<int16_t>(INT16_BIT); }
uint16_t ByteBuf::readUnsignedShortLE() {
  return d->readTLE<int16_t>(INT16_BIT);
}
int32_t ByteBuf::readInt() { return d->readT<int32_t>(INT32_BIT); }
int32_t ByteBuf::readIntLE() { return d->readTLE<int32_t>(INT32_BIT); }
uint32_t ByteBuf::readUnsignedInt() { return d->readT<int32_t>(INT32_BIT); }
uint32_t ByteBuf::readUnsignedIntLE() { return d->readTLE<int32_t>(INT32_BIT); }
int64_t ByteBuf::readLong() { return d->readT<int64_t>(INT64_BIT); }
int64_t ByteBuf::readLongLE() { return d->readTLE<int64_t>(INT64_BIT); }
uint64_t ByteBuf::readUnsignedLong() { return d->readT<int64_t>(INT64_BIT); }
uint64_t ByteBuf::readUnsignedLongLE() {
  return d->readTLE<int64_t>(INT64_BIT);
}
ByteBuf ByteBuf::readBytes(int32_t length) {
  std::string data = d->originData.substr(d->readerIndex, length);
  d->readerIndex += (int32_t)data.length();
  return ByteBuf(data);
}
double ByteBuf::readDouble() {
  double result = 0.00;
  if (d->originData.length() - d->readerIndex < DOUBLE_BIT) {
    return 0.00;
  }
  char *p = (char *)&result;
  for (; d->readerIndex < d->readerIndex + DOUBLE_BIT; d->readerIndex++) {
    *(p + d->readerIndex) = d->originData.at(d->readerIndex);
  }
  return result;
}
double ByteBuf::readDoubleLE() {
  double result = 0.00;
  if (d->originData.length() - d->readerIndex < DOUBLE_BIT) {
    return 0.00;
  }
  char *p = (char *)&result;
  for (; d->readerIndex < d->readerIndex + DOUBLE_BIT; d->readerIndex++) {
    *(p + d->readerIndex) = d->originData.at(DOUBLE_BIT - d->readerIndex - 1);
  }
  return result;
}
ByteBuf &ByteBuf::skipBytes(int32_t length) {
  d->readerIndex += length;
  if (d->readerIndex > d->originData.length()) {
    d->readerIndex = (int32_t)d->originData.length();
  }
  return *this;
}
ByteBuf &ByteBuf::setChar(int32_t index, int8_t value) {
  d->setT(index, value);
  return *this;
}
ByteBuf &ByteBuf::setUnsignedChar(int32_t index, uint8_t value) {
  d->setT(index, value);
  return *this;
}

ByteBuf &ByteBuf::setShort(int32_t index, int16_t value) {
  d->setT(index, value);
  return *this;
}
ByteBuf &ByteBuf::setShortLE(int32_t index, int16_t value) {
  d->setTLE(index, value);
  return *this;
}
ByteBuf &ByteBuf::setUnsignedShort(int32_t index, uint16_t value) {
  d->setT(index, value);
  return *this;
}
ByteBuf &ByteBuf::setUnsignedShortLE(int32_t index, uint16_t value) {
  d->setTLE(index, value);
  return *this;
}
ByteBuf &ByteBuf::setInt(int32_t index, int32_t value) {
  d->setT(index, value);
  return *this;
}
ByteBuf &ByteBuf::setIntLE(int32_t index, int32_t value) {
  d->setTLE(index, value);
  return *this;
}
ByteBuf &ByteBuf::setUnsignedInt(int32_t index, uint32_t value) {
  d->setT(index, value);
  return *this;
}
ByteBuf &ByteBuf::setUnsignedIntLE(int32_t index, uint32_t value) {
  d->setTLE(index, value);
  return *this;
}
ByteBuf &ByteBuf::setLong(int32_t index, int64_t value) {
  d->setT(index, value);
  return *this;
}
ByteBuf &ByteBuf::setLongLE(int32_t index, int64_t value) {
  d->setTLE(index, value);
  return *this;
}
ByteBuf &ByteBuf::setUnsignedLong(int32_t index, uint64_t value) {
  d->setT(index, value);
  return *this;
}
ByteBuf &ByteBuf::setUnsignedLongLE(int32_t index, uint64_t value) {
  d->setTLE(index, value);
  return *this;
}
ByteBuf &ByteBuf::setBytes(int32_t index, const ByteBuf &buf) {
  d->setData(index, buf.data());
  return *this;
}
ByteBuf &ByteBuf::setBytes(int32_t index, const char *data, int32_t size) {
  d->setData(index, std::string(data, size));
  return *this;
}

ByteBuf &ByteBuf::setZero(int32_t index, int32_t length) {
  d->setData(index, std::string(length, 0));
  return *this;
}
ByteBuf &ByteBuf::writeChar(int8_t value) {
  d->writeT(value);
  return *this;
}
ByteBuf &ByteBuf::writeUnsignedChar(uint8_t value) {
  d->writeT(value);
  return *this;
}
ByteBuf &ByteBuf::writeShort(int16_t value) {
  d->writeT(value);
  return *this;
}
ByteBuf &ByteBuf::writeShortLE(int16_t value) {
  d->writeTLE(value);
  return *this;
}
ByteBuf &ByteBuf::writeUnsignedShort(uint16_t value) {
  d->writeT(value);
  return *this;
}
ByteBuf &ByteBuf::writeUnsignedShortLE(uint16_t value) {
  d->writeTLE(value);
  return *this;
}
ByteBuf &ByteBuf::writeInt(int32_t value) {
  d->writeT(value);
  return *this;
}
ByteBuf &ByteBuf::writeIntLE(int32_t value) {
  d->writeTLE(value);
  return *this;
}
ByteBuf &ByteBuf::writeUnsignedInt(uint32_t value) {
  d->writeT(value);
  return *this;
}
ByteBuf &ByteBuf::writeUnsignedIntLE(uint32_t value) {
  d->writeTLE(value);
  return *this;
}
ByteBuf &ByteBuf::writeLong(int64_t value) {
  d->writeT(value);
  return *this;
}
ByteBuf &ByteBuf::writeLongLE(int64_t value) {
  d->writeTLE(value);
  return *this;
}
ByteBuf &ByteBuf::writeUnsignedLong(uint64_t value) {
  d->writeT(value);
  return *this;
}
ByteBuf &ByteBuf::writeUnsignedLongLE(uint64_t value) {
  d->writeTLE(value);
  return *this;
}
ByteBuf &ByteBuf::writeBytes(const ByteBuf &buf) {
  d->writeData(buf.data());
  return *this;
}
ByteBuf &ByteBuf::writeBytes(const char *data, int32_t size) {
  d->writeData(std::string(data, size));
  return *this;
}
ByteBuf &ByteBuf::writeZero(int32_t length) {
  d->writeData(std::string(length, 0));
  return *this;
}
char ByteBuf::toHexLower(uint32_t value) noexcept {
  return "0123456789abcdef"[value & 0xF];
}
char ByteBuf::toHexUpper(uint32_t value) noexcept {
  return "0123456789ABCDEF"[value & 0xF];
}
int32_t ByteBuf::fromHex(uint32_t c) noexcept {
  return ((c >= '0') && (c <= '9'))
             ? int32_t(c - '0')
             : ((c >= 'A') && (c <= 'F'))
                   ? int32_t(c - 'A' + 10)
                   : ((c >= 'a') && (c <= 'f')) ? int32_t(c - 'a' + 10) :
                                                /* otherwise */ -1;
}
ByteBuf ByteBuf::fromHex(const std::string &hexEncoded) noexcept {
  ByteBuf data;
  char ch = 0x00;
  for (int32_t index = 0; index < hexEncoded.length() - 1; index++) {
    ch |= (char)fromHex(hexEncoded.at(index++)) << 4;
    ch |= (char)fromHex(hexEncoded.at(index));
    data.writeChar(ch);
    ch = 0x00;
  }
  return data;
}

