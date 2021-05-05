#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "types.h"

#define KEY_UP 0x1F
#define KEY_DOWN 0x1E
#define KEY_RIGHT 0x1D
#define KEY_LEFT 0x1C

namespace Keyboard {
    void Init();
    Byte ReadInput();
    bool UpdateModifiers(Byte key, bool* ctrl, bool* alt, bool* shift);
};

extern "C" {
    void handleKeyboardInterrupt();
}

#endif

