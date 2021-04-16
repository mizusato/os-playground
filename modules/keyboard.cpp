#include "io.hpp"
#include "interrupt.hpp"
#include "scancode.hpp"
#include "keyboard.hpp"

const Byte ScanCodeMap[] = { SCANCODE_ASCII_TABLE };

void KeyboardInit() {
    InterruptSetup(0x21, (Number) KeyboardInterruptHandler, 0x08, 0x8E);
    InterruptUnmask(1);
}

bool KeyboardReady() {
    return (InputByte(0x64) & 0x1);
}

Byte KeyboardReadInput() {
    Number raw = (Number) InputByte(0x60);
    return ScanCodeMap[raw];
}

