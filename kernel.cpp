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
#include "ui/fonts.hpp"
#include "ui/windows.hpp"
#include "ui/widgets.hpp"
#include "ui/status.hpp"
#include "ui/log.hpp"
#include "ui/console.hpp"


extern "C"
void Main(MemoryInfo* memInfo, GraphicsInfo* gfxInfo) {
    static_cast<void>(static_cast<KernelEntryPoint>(Main));
    // Initialize
    Heap::Init(memInfo);
    Graphics::Init(gfxInfo);
    WindowManager::Init();
    Events::Init();
    Interrupt::Init();
    Panic::Init();
    Timer::Init();
    Keyboard::Init();
    Mouse::Init();
    // Create Windows
    Point screen_size(Graphics::ScreenWidth(), Graphics::ScreenHeight());
    BackgroundWindow background(Color(0x33, 0x33, 0xA3, 0xFF), screen_size);
    WindowManager::Add(&background);
    BaseWindow::Options opts;
    opts.closable = false;
    MemoryMonitor debug_memory(Point(230, 580), opts);
    TimerInspector debug_timer(Point(80, 600), opts);
    KeyboardInspector debug_keyboard(Point(100, 150), opts);
    MouseInspector debug_mouse(Point(60, 300), opts);
    LogViewer::Open(Point(700, 550), Point(300, 150), "Log Viewer", opts);
    Console::Open(Point(350, 100), Point(520, 320), "Console 0", opts);
    // First Rendering
    WindowManager::RenderAll(*Graphics::GetScreenCanvas());
    Graphics::FlushScreenCanvas();
    // Event Loop
    Point cursor_pos((screen_size.X / 2), (screen_size.Y / 2));
    bool event_emitted;
    MouseEvent prev_mouse_ev;
    prev_mouse_ev.pos = cursor_pos;
    while(true) {
        event_emitted = false;
        {
            TimerEvent ev;
            while (Events::TimerSecond->Read(&ev)) {
                event_emitted = true;
                debug_timer.Update(ev);
            }
        }
        {
            KeyboardEvent ev;
            while(Events::Keyboard->Read(&ev)) {
                event_emitted = true;
                WindowManager::DispatchEvent(ev);
                debug_keyboard.Update(ev);
            }
        }
        {
            MouseEvent ev;
            while(Events::Mouse->Read(&ev)) {
                event_emitted = true;
                Number x = cursor_pos.X;
                Number y = cursor_pos.Y;
                Number new_x = (x + ev.pos.X);
                Number new_y = (y + ev.pos.Y);
                if (
                    (new_x >= screen_size.X)
                    || (new_y >= screen_size.Y)
                    || ((new_x > x) && ((new_x - x) >= screen_size.X))
                    || ((x > new_x) && ((x - new_x) >= screen_size.X))
                    || ((new_y > y) && ((new_y - y) >= screen_size.Y))
                    || ((y > new_y) && ((y - new_y) >= screen_size.Y)) ) {
                    continue;
                }
                cursor_pos = Point(new_x, new_y);
                ev.pos = cursor_pos;
                if (!(prev_mouse_ev.btnLeft) && ev.btnLeft) { ev.down = true; }
                if (prev_mouse_ev.btnLeft && !(ev.btnLeft)) { ev.up = true; }
                WindowManager::DispatchEvent(ev, prev_mouse_ev);
                prev_mouse_ev = ev;
                debug_mouse.Update(ev);
            }
        }
        if (event_emitted) {
            debug_memory.Update(Heap::GetStatus());
            Canvas* canvas = Graphics::GetScreenCanvas();
            WindowManager::RenderAll(*canvas);
            for (Number dy = 0; dy < 36; dy += 1) {
                for (Number dx = 0; dx < 18; dx += 1) {
                    Number x = cursor_pos.X + dx;
                    Number y = cursor_pos.Y + dy;
                    canvas->DrawPixel(x, y, 0x33, 0xFF, 0xA0, 0xFF);
                }
            }
            Graphics::FlushScreenCanvas();
        }
        __asm__("hlt");
    };
}

extern "C" 
void __cxa_pure_virtual() {
    panic("pure virtual function called");
}

void handlePanicInterrupt() {
    Color red(0xFF, 0x33, 0x33, 0xFF);
    Color gray(0x33, 0x33, 0x33, 0xFF);
    Graphics::DrawString(150, 150, red, Panic::GetMessageTitle());
    Graphics::DrawString(150, 186, gray, Panic::GetMessageDetail());
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

bool ModCtrl = false;
bool ModAlt = false;
bool ModShift = false;

void handleKeyboardInterrupt() {
    Byte key = Keyboard::ReadInput();
    if (key != 0) {
        bool ok = Keyboard::UpdateModifiers(key, &ModCtrl, &ModAlt, &ModShift);
        if (ok) { return; }
        KeyboardEvent ev;
        ev.key = static_cast<Char>(key);
        ev.ctrl = ModCtrl;
        ev.alt = ModAlt;
        ev.shift = ModShift;
        Events::Keyboard->Write(ev);
    }
}

void handleMouseInterrupt() {
    Mouse::Packet packet;
    if (Mouse::ReadPacket(&packet)) {
        MouseEvent ev;
        ev.pos.X = packet.dx;
        ev.pos.Y = ((~ packet.dy) + 1);
        if (packet.buttons == Mouse::Button::Left) {
            ev.btnLeft = true;
        } else if (packet.buttons == Mouse::Button::Right) {
            ev.btnRight = true;
        }
        ev.ctrl = ModCtrl;
        ev.alt = ModAlt;
        Events::Mouse->Write(ev);
    }
}

