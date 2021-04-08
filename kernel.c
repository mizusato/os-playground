#include "lib/types.h"
#include "lib/string.h"
#include "lib/vga.h"
#include "lib/keyboard.h"
#include "lib/idt.h"
#include "lib/heap.h"


extern Void keyboard_irq_handler();

extern Void timer_irq_handler();

Void ShowHeapStatus();
Void TimerInit();

Void Main() {
    HeapInit();
    ClearScreen();
    ShowHeapStatus();
    WriteString(2, 4, StringFrom("Hello World"), ComposeColor(YELLOW, DARK_GRAY));
    MoveCursor(3, 4);
    IdtInit();
    IdtSetEntry(0x21, (Number) keyboard_irq_handler, 0x08, 0x8E);
    IdtSetEntry(0x20, (Number) timer_irq_handler, 0x08, 0x8E);
    KeyboardInit();
    TimerInit(18);
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
    HeapFree(chunk, 1);
    /* Send End of Interrupt (EOI) to master PIC */
    OutputByte(0x20, 0x20);
}

Void HandleTimer() {
    static Number counter = 0;
    static Number display = 0;
    static Byte buf[16];
    if ((counter % 18) == 0) {
        Color color = ComposeColor(YELLOW, MAGENTA);
        Number row = 21;
        Number col = 4;
        String s = StringFrom("Timer: ");
        WriteString(row, col, s, color);
        col += s.length;
        WriteString(row, col, StringFromNumber(display, buf), color);
        ShowHeapStatus();
        display += 1;
    }
    counter += 1;
    /* Send End of Interrupt (EOI) to master PIC */
    OutputByte(0x20, 0x20);
}

Void TimerInit(Number freq) {
    Number divisor = ((65535 * 18) / freq);
    OutputByte(0x43, 0x36);
    OutputByte(0x40, (divisor & 0xff));
    OutputByte(0x40, (divisor >> 8));
    OutputByte(0x21, (InputByte(0x21) & 0xfe));
}

