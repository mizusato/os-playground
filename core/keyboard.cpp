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
        Byte first = PS2::ReadData();
        if (first != 0xE0) {
            return ScanCodeMap[first];
        } else {
            Byte second = PS2::ReadData();
            if (second == 0x48) { return KEY_UP; } else
            if (second == 0x50) { return KEY_DOWN; } else
            if (second == 0x4D) { return KEY_RIGHT; } else
            if (second == 0x4B) { return KEY_LEFT; } else {
                return 0;
            }
        }
    }
    bool UpdateModifiers(Byte key, bool* ctrl, bool* alt, bool* shift) {
        if (key == DN_C) { *ctrl = true; } else
        if (key == UP_C) { *ctrl = false; } else
        if (key == DN_A) { *alt = true; } else
        if (key == UP_A) { *alt = false; } else
        if (key == DN_S) { *shift = true; } else
        if (key == UP_S) { *shift = false; } else
        { return false; }
        return true;
    }
}

