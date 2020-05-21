#ifndef OS_KEYBOARD_H
#define OS_KEYBOARD_H

#include "types.h"
#include "io.h"
#include "scancode.h"

Void KeyboardInit();

Bool KeyboardReady();

Byte ReadKeyboard();

#endif  // OS_KEYBOARD_H

