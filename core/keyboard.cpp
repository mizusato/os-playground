#include "io.hpp"
#include "interrupt.hpp"
#include "scancode.hpp"
#include "keyboard.hpp"

const Byte ScanCodeMap[] = { SCANCODE_ASCII_TABLE };

extern "C" void KeyboardInterruptHandler();

namespace Keyboard {
    void Init() {
        Interrupt::Setup(0x21, (Number) KeyboardInterruptHandler, 0x08, 0x8E);
        Interrupt::Unmask(1);
    }
    Byte ReadInput() {
        Byte raw = InputByte(0x60);
        return ScanCodeMap[raw];
    }
}

