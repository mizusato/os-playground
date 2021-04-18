#include "boot.h"
#include "core/heap.hpp"
#include "core/graphics.hpp"
#include "core/interrupt.hpp"
#include "core/panic.hpp"
#include "core/keyboard.hpp"
#include "core/list.hpp"


extern "C" {
    void Main(GraphicsInfo* gfxInfo);
    void handlePanicInterrupt();
    void handleKeyboardInterrupt();
}
void DrawBackground();

void Main(GraphicsInfo* gfxInfo) {
    Heap::Init();
    Graphics::Init(gfxInfo);
    Interrupt::Init();
    Panic::Init();
    Keyboard::Init();
    DrawBackground();
    Byte stub[] = "stub";
    {
        String::Builder buf;
        buf.Write("stub address: 0x");
        buf.Write(String::Hex((Number) &stub));
        Graphics::DrawString(400, 100, buf.Collect());
    }
    List<Number> l;
    l.Append(1);
    l.Append(2);
    l.Append(3);
    Unique<List<Number>> l2(new List<Number>());
    l2->Append(4);
    l2->Append(5);
    l.AppendAll(std::move(l2));
    char ch = 0;
    for (auto it = l.Iterate(); it->HasCurrent(); it->Proceed()) {
        Number n = it->Current();
        ch = 'g' + static_cast<char>(n);
        char str[2] = { ch, 0 };
        Graphics::DrawString(200, 100+100*n, str);
    }
    while(1) __asm__("hlt");
}

void handlePanicInterrupt() {
    Graphics::DrawString(150, 150, Panic::GetMessageTitle());
    Graphics::DrawString(150, 186, Panic::GetMessageDetail());
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

