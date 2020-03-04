#ifndef OS_SYSTEM_H
#define OS_SYSTEM_H

#include "types.h"

Byte InputByte (Word port) {
  Byte value;
  __asm__ __volatile__ ("inb %1, %0" : "=a" (value) : "dN" (port));
  return value;
}

Void OutputByte (Word port, Byte data) {
  __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

#endif
