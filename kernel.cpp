#include "boot.h"
#include "core/heap.hpp"
#include "core/graphics.hpp"
#include "core/interrupt.hpp"
#include "core/panic.hpp"
#include "core/timer.hpp"
#include "core/keyboard.hpp"
#include "core/mouse.hpp"
#include "core/list.hpp"


extern "C" {
    void Main(MemoryInfo* memInfo, GraphicsInfo* gfxInfo);
    void handlePanicInterrupt();
    void handleTimerInterrupt();
    void handleKeyboardInterrupt();
    void handleMouseInterrupt();
}
void DrawBackground();

void Main(MemoryInfo* memInfo, GraphicsInfo* gfxInfo) {
    static_cast<void>(static_cast<KernelEntryPoint>(Main));
    Heap::Init(memInfo);
    Graphics::Init(gfxInfo);
    Interrupt::Init();
    Panic::Init();
    Timer::Init();
    Keyboard::Init();
    Mouse::Init();
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
        buf.Write("(efi available)");
        buf.Write("\n");
        buf.Write("addr / size / kind");
        buf.Write("\n");
        Number base = (Number) memInfo->mapBuffer;
        for (Number ptr = base; (ptr - base) < memInfo->mapSize; ptr += memInfo->descSize) {
            MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(ptr);
            if ( ! ((desc->physicalStart >= 0x100000) &&
                (desc->numberOfPages >= 256) &&
                (desc->kind.value == MK_ConventionalMemory)) ) {
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
    {
        HeapStatus status = Heap::GetStatus();
        String::Builder buf;
        buf.Write("(consumed as static)");
        buf.Write("\n");
        buf.Write("addr / size");
        buf.Write("\n");
        buf.Write(String::ReadableSize(status.StaticPosition));
        buf.Write(" / ");
        buf.Write(String::ReadableSize(status.StaticSize));
        buf.Write("\n");
        buf.Write("(consumed as chunks)");
        buf.Write("\n");
        buf.Write("addr / size");
        buf.Write("\n");
        Number n;
        const HeapMemoryInfo* info = Heap::GetInfo(&n);
        for (Number i = 0; i < n; i += 1) {
            buf.Write(String::ReadableSize(info[i].start));
            buf.Write(" / ");
            buf.Write(String::ReadableSize(info[i].size));
            buf.Write("\n");
        }
        Graphics::DrawString(300, 400, buf.Collect());
    }
    {
        String::Builder buf;
        HeapStatus status = Heap::GetStatus();
        buf.Write("heap chunks: ");
        buf.Write(String(status.ChunksTotal));
        buf.Write("\n");
        buf.Write("heap size: ");
        buf.Write(String::ReadableSize(status.ChunksTotal * sizeof(Chunk)));
        Graphics::DrawString(300, 675, buf.Collect());
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

void handleTimerInterrupt() {
    static Number seconds = 0;
    static Number count = 0;
    if (count == 0) {
        Graphics::DrawString(750, 600, String(seconds));
        seconds += 1;
    }
    count = (count + 1) % 250;
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

void handleMouseInterrupt() {
    Mouse::Packet packet;
    Number count = 0;
    while (Mouse::ReadPacket(&packet)) {
        String::Builder buf;
        buf.Write("mouse:");
        buf.Write("\n");
        buf.Write("dx: ");
        buf.Write(String::Pad(String::Hex(packet.dx), 16));
        buf.Write("\n");
        buf.Write("dy: ");
        buf.Write(String::Pad(String::Hex(packet.dy), 16));
        Graphics::DrawString(100, 200, buf.Collect());
        count += 1;
    }
    String::Builder buf;
    buf.Write(String(count));
    buf.Write(" packets received");
    Graphics::DrawString(100, 350, buf.Collect());
}

void DrawBackground() {
    for (Number y = 0; y < Graphics::ScreenHeight(); y += 1) {
        for (Number x = 0; x < Graphics::ScreenWidth(); x += 1) {
            Graphics::DrawPixel(x, y, 255, (x % 256), 64);
        }
    }
    Graphics::DrawString(100, 100, "hello world");
}

