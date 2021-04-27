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


class BackgroundWindow final: public Window {
public:
    BackgroundWindow(Point size): Window(Point(0,0), size) {};
    ~BackgroundWindow() {};
    void Render(Canvas& target, bool _) override {
        Number h = target.Height();
        Number w = target.Width();
        for (Number y = 0; y < h; y += 1) {
            for (Number x = 0; x < w; x += 1) {
                target.DrawPixel(x, y, 255, (x % 256), 64, 0xFF);
            }
        }
    }
    void DispatchEvent(KeyboardEvent ev) override {}
    void DispatchEvent(MouseEvent ev) override {}
};

class TextWindow final: public Window {
private:
    String text;
public:
    TextWindow(Point position, Point size): Window(position, size), text("") {};
    ~TextWindow() {};
    void SetText(String new_text) {
        text = new_text;
    }
    void Render(Canvas& target, bool active) override {
        Color bg_inactive(0xFF, 0xFF, 0xFF, 0xFF);
        Color bg_active(0xFE, 0xFE, 0xCD, 0xFF);
        Color bg_titlebar(0x33, 0x33, 0xFF, 0xFF);
        Number titlebar_size = 16;
        Color bg = bg_inactive;
        if (active) { bg = bg_active; }
        Number h = target.Height();
        Number w = target.Width();
        for (Number y = 0; y < h; y += 1) {
            for (Number x = 0; x < w; x += 1) {
                if (y < titlebar_size) {
                    target.DrawPixel(x, y, bg_titlebar.R, bg_titlebar.G, bg_titlebar.B, bg_titlebar.A);
                } else {
                    target.DrawPixel(x, y, bg.R, bg.G, bg.B, bg.A);
                }
            }
        }
        Color fg(0, 0, 0, 0xFF);
        Font* font = Graphics::GetBasicFont();
        target.FillText(Point(0,titlebar_size), fg, bg, *font, text);
    }
    void DispatchEvent(KeyboardEvent ev) override {}
    void DispatchEvent(MouseEvent ev) override {}
    static TextWindow* Add(Point pos, Point size, String text) {
        auto w = new TextWindow(pos, size);
        w->SetText(text);
        WindowManager::Add(w);
        return w;
    }
};

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
    BackgroundWindow win_bg(Point(Graphics::ScreenWidth(), Graphics::ScreenHeight()));
    WindowManager::Add(&win_bg);
    Byte stub[] = "stub";
    {
        String::Builder buf;
        buf.Write("framebuffer addr: ");
        buf.Write(String::ReadableSize((Number) gfxInfo->framebuffer));
        buf.Write("\n");
        buf.Write("stub addr: ");
        buf.Write(String::ReadableSize((Number) &stub));
        TextWindow::Add(Point(400, 100), Point(500, 90), buf.Collect());
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
        TextWindow::Add(Point(350, 200), Point(600, 200), buf.Collect());
    }
    {
        HeapStatus status = Heap::GetStatus();
        String::Builder buf;
        buf.Write("(consumed as static)");
        buf.Write("\n");
        buf.Write(String::ReadableSize(status.StaticPosition));
        buf.Write(" (");
        buf.Write(String::ReadableSize(status.StaticSize));
        buf.Write(")\n");
        buf.Write("(consumed as chunks)");
        buf.Write("\n");
        Number n;
        const HeapMemoryInfo* info = Heap::GetInfo(&n);
        for (Number i = 0; i < n; i += 1) {
            buf.Write(String::ReadableSize(info[i].start));
            buf.Write(" (");
            buf.Write(String::ReadableSize(info[i].size));
            buf.Write(")\n");
        }
        TextWindow::Add(Point(320, 400), Point(600, 240), buf.Collect());
    }
    {
        String::Builder buf;
        HeapStatus status = Heap::GetStatus();
        buf.Write("heap chunks: ");
        buf.Write(String(status.ChunksTotal));
        buf.Write("\n");
        buf.Write("heap size: ");
        buf.Write(String::ReadableSize(status.ChunksTotal * sizeof(Chunk)));
        TextWindow::Add(Point(280,660), Point(400, 90), buf.Collect());
    }
    auto win_timer = TextWindow::Add(Point(80, 600), Point(100, 60), "");
    auto win_keyboard = TextWindow::Add(Point(100, 150), Point(200, 50), "");
    auto win_mouse = TextWindow::Add(Point(60, 250), Point(240, 150), "");
    WindowManager::RenderAll(*Graphics::GetScreenCanvas());
    Graphics::FlushScreenCanvas();
    bool event_emitted;
    while(true) {
        event_emitted = false;
        {
            TimerEvent ev;
            while (Events::TimerSecond->Read(&ev)) {
                event_emitted = true;
                win_timer->SetText(String(ev.count));
            }
        }
        {
            KeyboardEvent ev;
            while(Events::Keyboard->Read(&ev)) {
                event_emitted = true;
                Char key = ev.key;
                if (key == 0) {
                    continue;
                }
                String::Builder buf;
                buf.Write(String::Chr(key));
                buf.Write(" ");
                buf.Write(String::Hex(key));
                win_keyboard->SetText(buf.Collect());
            }
        }
        {
            MouseEvent ev;
            while(Events::Mouse->Read(&ev)) {
                event_emitted = true;
                String::Builder buf;
                buf.Write("mouse:");
                buf.Write("\n");
                buf.Write("dx: ");
                buf.Write(String::Pad(String::Hex(ev.pos.X), 16));
                buf.Write("\n");
                buf.Write("dy: ");
                buf.Write(String::Pad(String::Hex(ev.pos.Y), 16));
                win_mouse->SetText(buf.Collect());
            }
        }
        if (event_emitted) {
            WindowManager::RenderAll(*Graphics::GetScreenCanvas());
            Graphics::FlushScreenCanvas();
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

