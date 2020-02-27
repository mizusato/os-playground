#ifndef OS_STRING_H
#define OS_STRING_H

typedef struct {
  Size         length;
  const Byte*  chars;
} String;

String StringFrom(const Byte* raw) {
  const Byte* ptr = raw;
  Size length = 0;
  while (*ptr != 0) {
    length += 1;
    ptr += 1;
  }
  String result = { length, raw };
  return result;
}

#endif
