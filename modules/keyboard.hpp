#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "types.h"

void KeyboardInit();
Byte KeyboardReadInput();

extern "C" {
    void KeyboardInterruptHandler();
    void handleKeyboardInterrupt();
}

#endif
