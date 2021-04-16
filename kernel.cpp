#include "boot.h"
#include "modules/heap.hpp"
#include "modules/graphics.hpp"
#include "modules/interrupt.hpp"
#include "modules/keyboard.hpp"


extern "C" {
    void Main(GraphicsInfo* gfxInfo);
    void handleKeyboardInterrupt();
}
void DrawBackground();

void Main(GraphicsInfo* gfxInfo) {
    Heap::Init();
    Graphics::Init(gfxInfo);
    Interrupt::Init();
    Keyboard::Init();
    DrawBackground();
    while(1) __asm__("hlt");
}

void handleKeyboardInterrupt() {
    Byte key = Keyboard::ReadInput();
    char str[2] = { key, 0 };
    Graphics::DrawString(100, 150, str);
}

void DrawBackground() {
    for (Number y = 0; y < Graphics::ScreenHeight(); y += 1) {
        for (Number x = 0; x < Graphics::ScreenWidth(); x += 1) {
            Graphics::DrawPixel(x, y, 255, (x % 256), 64);
        }
    }
    Graphics::DrawString(100, 100, "hello world");
}

