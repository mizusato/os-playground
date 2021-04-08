#include "lib/types.h"
#include "lib/string.h"
#include "lib/vga.h"
#include "lib/keyboard.h"
#include "lib/idt.h"
#include "lib/heap.h"


extern Void keyboard_irq_handler();

Void ShowHeapStatus();

Void Main() {
    HeapInit();
    ClearScreen();
    ShowHeapStatus();
    WriteScreenString(0, 0, StringFrom("Hello World"), ComposeColor(YELLOW, DARK_GRAY));
    MoveCursor(3, 5);
    IdtInit();
    IdtSetEntry(0x21, (Size) keyboard_irq_handler, 0x08, 0x8E);
    KeyboardInit();
}

Void ShowHeapStatus() {
    Size row = 24;
    Color color = ComposeColor(YELLOW, MAGENTA);
    ClearRow(row);
    Chunk* chunk = HeapAllocate(1);
    Byte* buf = chunk->data;
    HeapStatus status = HeapGetStatus();
    String s = DecimalString(status.ChunksAvailable, buf);
    WriteScreenString(row, 0, s, color);
    Size col = s.length;
    WriteScreenString(row, col, StringFrom("/"), color);
    col += 1;
    s = DecimalString(status.ChunksTotal, buf);
    WriteScreenString(row, col, s, color);
    col += s.length;
    HeapFree(chunk, 1);
}

Void HandleKeyboardInput() {
    Chunk* chunk = HeapAllocate(1);
    Byte* buf = chunk->data;
    Byte key = ReadKeyboard();
    if (key != 0) {
        String s = DecimalString(key, buf);
        ClearRow(0);
        WriteScreenString(0, 0, s, BRIGHT_CYAN);
        WriteScreen(0, (s.length + 1), key, YELLOW);
    }
    /* Send End of Interrupt (EOI) to master PIC */
    OutputByte(0x20, 0x20);
    HeapFree(chunk, 1);
    ShowHeapStatus();
}

