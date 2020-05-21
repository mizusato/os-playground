#include "io.h"
#include "keyboard.h"


const Byte ScanCodeMap[] = { SCANCODE_ASCII_TABLE };

Void KeyboardInit() {
    Byte mask = GetIrqMask();
    SetIrqMask(mask & (~(0x1 << 1)));
}

Bool KeyboardReady() {
    return (InputByte(0x64) & 0x1);
}

Byte ReadKeyboard() {
    Byte raw = InputByte(0x60);
    return ScanCodeMap[raw];
}

