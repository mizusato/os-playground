#include "interrupt.hpp"
#include "ps2.hpp"
#include "scancode.hpp"
#include "keyboard.hpp"


const Byte ScanCodeMap[] = { SCANCODE_ASCII_TABLE };

extern "C" void KeyboardInterruptHandler();

namespace Keyboard {
    void Init() {
        Interrupt::Setup(IRQ(1), KeyboardInterruptHandler);
        Interrupt::UnmaskIRQ(1);
    }
    Byte ReadInput() {
        return ScanCodeMap[PS2::ReadData()];
    }
}

