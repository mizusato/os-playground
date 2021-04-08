#include "string.h"


String StringFrom(const char* raw) {
  const char* ptr = raw;
  Number length = 0;
  while (*ptr != 0) {
    length += 1;
    ptr += 1;
  }
  String result = { length, (const Byte*)(raw) };
  return result;
}


String StringFromNumber(Number number, Byte* buf) {
  if (number == 0) {
    buf[0] = '0';
    String result = { 1, buf };
    return result;
  }
  Number q = number;
  Number r = 0;
  Byte* ptr = buf;
  Number length = 0;
  while (q > 0) {
    r = q % 10;
    q = q / 10;
    *ptr = ('0' + r);
    ptr += 1;
    length += 1;
  }
  Number i;
  for (i = 0; i < (length / 2); i += 1) {
    Number opposite = length-1-i;
    Byte t = buf[i];
    buf[i] = buf[opposite];
    buf[opposite] = t;
  }
  String result = { length, buf };
  return result;
}
