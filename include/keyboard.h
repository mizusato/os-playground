#ifndef OS_KEYBOARD_H
#define OS_KEYBOARD_H

#include "scancode.h"

const Byte ScanCodeMap[] = { SCANCODE_ASCII_TABLE };

Bool KeyboardReady() {
  return (InPortB(0x64) & 0x1);
}

Byte ReadKeyboard() {
  Byte raw = InPortB(0x60);
  return ScanCodeMap[raw];
}

#endif
