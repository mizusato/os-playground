#ifndef OS_KEYBOARD_H
#define OS_KEYBOARD_H

#include "types.h"
#include "system.h"
#include "scancode.h"

const Byte ScanCodeMap[] = { SCANCODE_ASCII_TABLE };

Bool KeyboardReady() {
  return (InputByte(0x64) & 0x1);
}

Byte ReadKeyboard() {
  Byte raw = InputByte(0x60);
  return ScanCodeMap[raw];
}

#endif
