#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "types.h"

namespace Keyboard {
    void Init();
    Byte ReadInput();
    void UpdateModifiers(Byte key, bool* ctrl, bool* alt, bool* shift);
};

extern "C" {
    void handleKeyboardInterrupt();
}

#endif

