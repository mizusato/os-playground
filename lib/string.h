#ifndef OS_STRING_H
#define OS_STRING_H

#include "types.h"

typedef struct {
  Number         length;
  const Byte*  chars;
} String;

String StringFrom(const char* raw);

String StringFromNumber(Number number, Byte* buf);


#endif  // OS_STRING_H
