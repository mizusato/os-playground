#ifndef OS_IO_H
#define OS_IO_H

#include "types.h"

// These kind of low-level I/O functions can be found in <sys/io.h>

static __inline
Byte InputByte (Word port) {
  Byte value;
  __asm__ __volatile__ ("inb %1, %0" : "=a" (value) : "dN" (port));
  return value;
}

static __inline
Void OutputByte (Word port, Byte data) {
  __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}


#endif  // OS_IO_H
