#ifndef OS_IO_H
#define OS_IO_H

#include "types.h"

// Some low-level I/O functions can be found in <sys/io.h>

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

static __inline
Byte GetIrqMask() {
  return InputByte(0x21);
};

static __inline
Void SetIrqMask(Byte mask) {
  OutputByte(0x21, mask);
};

#endif  // OS_IO_H

