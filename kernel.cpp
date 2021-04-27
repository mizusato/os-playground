#include "boot.h"
#include "core/heap.hpp"
#include "core/graphics.hpp"
#include "core/window.hpp"
#include "core/events.hpp"
#include "core/interrupt.hpp"
#include "core/panic.hpp"
#include "core/timer.hpp"
#include "core/keyboard.hpp"
#include "core/mouse.hpp"
#include "core/list.hpp"


void DrawBackground();

extern "C"
void Main(MemoryInfo* memInfo, GraphicsInfo* gfxInfo) {
    static_cast<void>(static_cast<KernelEntryPoint>(Main));
    Heap::Init(memInfo);
    Graphics::Init(gfxInfo);
    WindowManager::Init();
    Events::Init();
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
    while(true) {
        {
            TimerEvent ev;
            while (Events::TimerSecond->Read(&ev)) {
                Graphics::DrawString(750, 600, String(ev.count));
            }
        }
        {
            KeyboardEvent ev;
            while(Events::Keyboard->Read(&ev)) {
                Char key = ev.key;
                if (key == 0) {
                    continue;
                }
                String::Builder buf;
                buf.Write(String::Chr(key));
                buf.Write(" ");
                buf.Write(String::Hex(key));
                Graphics::DrawString(100, 150, "    ");
                Graphics::DrawString(100, 150, buf.Collect());
            }
        }
        {
            MouseEvent ev;
            while(Events::Mouse->Read(&ev)) {
                String::Builder buf;
                buf.Write("mouse:");
                buf.Write("\n");
                buf.Write("dx: ");
                buf.Write(String::Pad(String::Hex(ev.pos.X), 16));
                buf.Write("\n");
                buf.Write("dy: ");
                buf.Write(String::Pad(String::Hex(ev.pos.Y), 16));
                Graphics::DrawString(100, 200, buf.Collect());
            }
        }
        __asm__("hlt");
    };
}

void handlePanicInterrupt() {
    Graphics::DrawString(150, 150, Panic::GetMessageTitle());
    Graphics::DrawString(150, 186, Panic::GetMessageDetail());
}

void handleTimerInterrupt() {
    static Number seconds = 0;
    static Number count = 0;
    if (count == 0) {
        TimerEvent ev;
        ev.count = seconds;
        Events::TimerSecond->Write(ev);
        seconds += 1;
    }
    count = (count + 1) % 250;
}

void handleKeyboardInterrupt() {
    Byte key = Keyboard::ReadInput();
    KeyboardEvent ev;
    ev.key = static_cast<Char>(key);
    Events::Keyboard->Write(ev);
}

void handleMouseInterrupt() {
    Mouse::Packet packet;
    if (Mouse::ReadPacket(&packet)) {
        MouseEvent ev;
        ev.pos.X = packet.dx;
        ev.pos.Y = packet.dy;
        if (packet.buttons == Mouse::Button::Left) {
            ev.button = 0;
        } else if (packet.buttons == Mouse::Button::Right) {
            ev.button = 1;
        } else {
            ev.button = 0xFF;
        }
        Events::Mouse->Write(ev);
    }
}

void DrawBackground() {
    for (Number y = 0; y < Graphics::ScreenHeight(); y += 1) {
        for (Number x = 0; x < Graphics::ScreenWidth(); x += 1) {
            Graphics::DrawPixel(x, y, 255, (x % 256), 64);
        }
    }
    Graphics::DrawString(100, 100, "hello world");
}

