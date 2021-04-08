#include "lib/types.h"
#include "lib/string.h"
#include "lib/vga.h"
#include "lib/keyboard.h"
#include "lib/idt.h"


extern void keyboard_irq_handler();

Void Main() {
    ClearScreen();
    WriteScreenString(0, 0, StringFrom("Hello World"), YELLOW);
    MoveCursor(3, 5);
    IdtInit();
    IdtSetEntry(0x21, (Size) keyboard_irq_handler, 0x08, 0x8E);
    KeyboardInit();
}

Void HandleKeyboardInput() {
    Byte buf[100];
    Byte key = ReadKeyboard();
    if (key != 0) {
        String str = DecimalString(key, buf);
        ClearScreen();
        WriteScreenString(0, 0, str, BRIGHT_CYAN);
        WriteScreen(0, (str.length + 1), key, YELLOW);
    }
    /* Send End of Interrupt (EOI) to master PIC */
    OutputByte(0x20, 0x20);
}

