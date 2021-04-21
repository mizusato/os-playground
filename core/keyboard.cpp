#include "interrupt.hpp"
#include "ps2.hpp"
#include "scancode.hpp"
#include "keyboard.hpp"


const Byte ScanCodeMap[] = { SCANCODE_ASCII_TABLE };

extern "C" void KeyboardInterruptHandler();

namespace Keyboard {
    void Init() {
        Interrupt::Setup(0x21, (Number) KeyboardInterruptHandler, 0x08, 0x8E);
        Interrupt::UnmaskPIC1(1);
    }
    Byte ReadInput() {
        return ScanCodeMap[PS2::ReadData()];
    }
}

