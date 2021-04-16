#include "boot.h"
#include "modules/heap.hpp"
#include "modules/gfx.hpp"
#include "modules/interrupt.hpp"
#include "modules/keyboard.hpp"


extern "C" {
    void Main(GraphicsInfo* gfxInfo);
    void handleKeyboardInterrupt();
}
void DrawBackground();

void Main(GraphicsInfo* gfxInfo) {
    Heap::Init();
    Screen::Init(gfxInfo);
    Interrupt::Init();
    Keyboard::Init();
    DrawBackground();
    while(1) __asm__("hlt");
}

void handleKeyboardInterrupt() {
    Byte key = Keyboard::ReadInput();
    char str[2] = { key, 0 };
    Screen::Instance()->DrawString(100, 150, str);
    Interrupt::NotifyHandled();
}

void DrawBackground() {
    Screen* screen = Screen::Instance();
    for (Number y = 0; y < screen->Height(); y += 1) {
        for (Number x = 0; x < screen->Width(); x += 1) {
            screen->DrawPixel(x, y, 255, (x % 256), 64);
        }
    }
    screen->DrawString(100, 100, "hello world");
}

