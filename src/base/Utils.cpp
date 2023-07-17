#include "Utils.h"
const char Utils::digits[] = "9876543210123456789";
const char* Utils::zero = digits + 9;
const char Utils::digitsHex[] = "0123456789ABCDEF";

size_t Utils::convertHex(char buf[], uintptr_t value) {
  uintptr_t i = value;
  char* p = buf;
  do {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);
  *p = '\0';
  std::reverse(buf, p);
  return p - buf;
}
/***********************************************************************************************/
