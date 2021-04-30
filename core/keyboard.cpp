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
    void UpdateModifiers(Byte key, bool* ctrl, bool* alt, bool* shift) {
        if (key == DN_C) { *ctrl = true; } else
        if (key == UP_C) { *ctrl = false; } else
        if (key == DN_A) { *alt = true; } else
        if (key == UP_A) { *alt = false; } else
        if (key == DN_S) { *shift = true; } else
        if (key == UP_S) { *shift = false; }
    }
}

