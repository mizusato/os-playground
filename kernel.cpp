#include "boot.h"
#include "modules/heap.hpp"
#include "modules/graphics.hpp"
#include "modules/interrupt.hpp"
#include "modules/keyboard.hpp"
#include "modules/list.hpp"


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
    List<Number> l;
    l.Append(1);
    l.Append(2);
    l.Append(3);
    Unique<List<Number>> l2(new List<Number>());
    l2->Append(4);
    l2->Append(5);
    l.AppendAll(std::move(l2));
    char ch = 0;
    for (auto it = l.Iterate(); it->HasValue(); it->Proceed()) {
        Number n = it->Value();
        ch = 'g' + static_cast<char>(n);
        char str[2] = { ch, 0 };
        Graphics::DrawString(200, 100+100*n, str);
    }
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

