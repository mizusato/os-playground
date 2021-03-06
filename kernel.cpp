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
#include "core/scheduler.hpp"
#include "ui/fonts.hpp"
#include "ui/cursor.hpp"
#include "ui/windows.hpp"
#include "ui/widgets.hpp"
#include "ui/status.hpp"
#include "ui/log.hpp"
#include "ui/console.hpp"
#include "ui/ui.hpp"


#define MAX_INTENSIVE_CYCLES 64

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
    Scheduler::Init();
    // Create Windows
    Point screenSize(Graphics::ScreenWidth(), Graphics::ScreenHeight());
    BaseWindow::Options opts;
    opts.closable = false;
    BackgroundWindow background(Color(0x33, 0x33, 0xA3, 0xFF), screenSize);
    MemoryMonitor memoryMonitor(Point(230, 590), opts);
    LogViewer::Open(Point(700, 550), Point(300, 150), "Log Viewer", opts);
    Console::Open(Point(50, 100), opts);
    // First Rendering
    Cursor::SetPosition(Point((screenSize.X / 2), (screenSize.Y / 2)));
    memoryMonitor.Update(Heap::GetStatus());
    log("initialized", LL_Info);
    RenderUI();
    // Event Loop
    TaskScheduler::CycleInfo cycleInfo;
    Number intensiveCycleCount = 0;
    while(true) {
        bool significantEventEmitted = ProcessEvents();
        Scheduler::Cycle(&cycleInfo);
        bool somethingExecuted = cycleInfo.somethingExecuted;
        bool tasksChanged = cycleInfo.tasksChanged;
        intensiveCycleCount += 1;
        if (somethingExecuted && !(tasksChanged) && !(significantEventEmitted)) {
            if (intensiveCycleCount < MAX_INTENSIVE_CYCLES) {
                continue;
            }
        }
        intensiveCycleCount = 0;
        if (somethingExecuted || tasksChanged || significantEventEmitted) {
            memoryMonitor.Update(Heap::GetStatus());
            RenderUI();
        } else {
            __asm__("hlt");
        }
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
    // 4ms interval
    static Number event_count = 0;
    static Number tick = 0;
    if (tick == 0) {
        // 100ms interval
        TimerEvent ev;
        ev.count = event_count;
        event_count += 1;
        Events::Timer->Write(ev);
    }
    tick = (tick + 1) % 25;
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

