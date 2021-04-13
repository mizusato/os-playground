#include "boot.h"
#include "modules/gfx.hpp"

extern "C"
void Main(GraphicsInfo* gfxInfo) {
    HeapInit();
    auto screen = Screen::From(gfxInfo);
    for (Number y = 0; y < screen->Height(); y += 1) {
        for (Number x = 0; x < screen->Width(); x += 1) {
            screen->DrawPixel(x, y, 255, (x % 256), 64);
        }
    }
    screen->DrawString(100, 100, "hello world");
    while(1) __asm__("hlt");
}

