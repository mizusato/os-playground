#include "string.h"


String StringFrom(const char* raw) {
  const char* ptr = raw;
  Size length = 0;
  while (*ptr != 0) {
    length += 1;
    ptr += 1;
  }
  String result = { length, (const Byte*)(raw) };
  return result;
}


String DecimalString(Size number, Byte* buf) {
  if (number == 0) {
    buf[0] = '0';
    String result = { 1, buf };
    return result;
  }
  Size q = number;
  Size r = 0;
  Byte* ptr = buf;
  Size length = 0;
  while (q > 0) {
    r = q % 10;
    q = q / 10;
    *ptr = ('0' + r);
    ptr += 1;
    length += 1;
  }
  Size i;
  for (i = 0; i < (length / 2); i += 1) {
    Size opposite = length-1-i;
    Byte t = buf[i];
    buf[i] = buf[opposite];
    buf[opposite] = t;
  }
  String result = { length, buf };
  return result;
}
