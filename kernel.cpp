#include "boot.h"
#include "core/heap.hpp"
#include "core/graphics.hpp"
#include "core/interrupt.hpp"
#include "core/panic.hpp"
#include "core/keyboard.hpp"
#include "core/list.hpp"


extern "C" {
    void Main(MemoryInfo* memInfo, GraphicsInfo* gfxInfo);
    void handlePanicInterrupt();
    void handleKeyboardInterrupt();
}
void DrawBackground();

void Main(MemoryInfo* memInfo, GraphicsInfo* gfxInfo) {
    static_cast<void>(static_cast<KernelEntryPoint>(Main));
    Heap::Init();
    Graphics::Init(gfxInfo);
    Interrupt::Init();
    Panic::Init();
    Keyboard::Init();
    DrawBackground();
    Byte stub[] = "stub";
    {
        String::Builder buf;
        buf.Write("framebuffer addr: ");
        buf.Write(String::ReadableSize((Number) gfxInfo->framebuffer));
        buf.Write("\n");
        buf.Write("stub addr: ");
        buf.Write(String::ReadableSize((Number) &stub));
        Graphics::DrawString(400, 100, buf.Collect());
    }
    {
        static const char* MemoryKindNames[] = { MEMORY_KIND_NAMES };
        String::Builder buf;
        buf.Write("addr / size / kind");
        buf.Write("\n");
        Number base = (Number) memInfo->mapBuffer;
        for (Number ptr = base; (ptr - base) < memInfo->mapSize; ptr += memInfo->descSize) {
            MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(ptr);
            if ((desc->physicalStart < 0x100000) || (desc->numberOfPages < 256)) {
                continue;
            }
            const char* kindName = MemoryKindNames[desc->kind.raw];
            buf.Write(String::ReadableSize((Number) desc->physicalStart));
            buf.Write(" / ");
            buf.Write(String::ReadableSize((Number) (desc->numberOfPages * 4096)));
            buf.Write(" / ");
            buf.Write(kindName);
            buf.Write("\n");
        }
        Graphics::DrawString(300, 200, buf.Collect());
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
    if (key == 0) { return; }
    String::Builder buf;
    buf.Write(String::Chr(key));
    buf.Write(" ");
    buf.Write(String::Hex(key));
    Graphics::DrawString(100, 150, "    ");
    Graphics::DrawString(100, 150, buf.Collect());
}

void DrawBackground() {
    for (Number y = 0; y < Graphics::ScreenHeight(); y += 1) {
        for (Number x = 0; x < Graphics::ScreenWidth(); x += 1) {
            Graphics::DrawPixel(x, y, 255, (x % 256), 64);
        }
    }
    Graphics::DrawString(100, 100, "hello world");
}

