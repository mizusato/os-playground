#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "types.h"

namespace Keyboard {
    void Init();
    Byte ReadInput();
};

extern "C" {
    void handleKeyboardInterrupt();
}

#endif

