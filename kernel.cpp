#include "boot.h"
#include "modules/heap.hpp"
#include "modules/gfx.hpp"
#include "modules/interrupt.hpp"
#include "modules/keyboard.hpp"


Screen* screen;

extern "C" {
    void Main(GraphicsInfo* gfxInfo) {
        HeapInit();
        screen = Screen::From(gfxInfo);
        InterruptInit();
        KeyboardInit();
        for (Number y = 0; y < screen->Height(); y += 1) {
            for (Number x = 0; x < screen->Width(); x += 1) {
                screen->DrawPixel(x, y, 255, (x % 256), 64);
            }
        }
        screen->DrawString(100, 100, "hello world");
        while(1) __asm__("hlt");
    }
    void handleKeyboardInterrupt() {
        Byte key = KeyboardReadInput();
        char str[2] = { key, 0 };
        screen->DrawString(100, 150, str);
        InterruptNotifyHandled();
    }
}

