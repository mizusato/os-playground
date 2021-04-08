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
    WriteString(2, 4, StringFrom("Hello World"), ComposeColor(YELLOW, DARK_GRAY));
    MoveCursor(3, 4);
    IdtInit();
    IdtSetEntry(0x21, (Number) keyboard_irq_handler, 0x08, 0x8E);
    KeyboardInit();
}

Void ShowHeapStatus() {
    Chunk* chunk = HeapAllocate(1);
    Byte* buf = chunk->data;
    Number row = 22;
    Number col = 4;
    String s;
    Color color = ComposeColor(YELLOW, MAGENTA);
    #define WRITE(str) \
        s = (str); \
        WriteString(row, col, s, color); \
        col += s.length;
    HeapStatus status = HeapGetStatus();
    ClearRow(row);
    WRITE(StringFrom("Heap Used Chunks: "));
    WRITE(StringFromNumber(status.ChunksAvailable, buf));
    WRITE(StringFrom("/"));
    WRITE(StringFromNumber(status.ChunksTotal, buf));
    HeapFree(chunk, 1);
}

Void HandleKeyboardInput() {
    Chunk* chunk = HeapAllocate(1);
    Byte* buf = chunk->data;
    Byte key = ReadKeyboard();
    if (key != 0) {
        String s = StringFromNumber(key, buf);
        ClearRow(2);
        WriteChar(2, 4, key, YELLOW);
        WriteString(2, 6, s, BRIGHT_CYAN);
    }
    /* Send End of Interrupt (EOI) to master PIC */
    OutputByte(0x20, 0x20);
    HeapFree(chunk, 1);
    ShowHeapStatus();
}

