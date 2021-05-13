#ifndef UI_UI_HPP
#define UI_UI_HPP

#include "../core/graphics.hpp"
#include "../core/window.hpp"
#include "cursor.hpp"

bool ProcessEvents() {
    bool significantEventEmitted = false;
    {
        TimerEvent ev;
        while (Events::Timer->Read(&ev)) {
            bool consumed = Scheduler::DispatchTimerEvent(ev);
            if (consumed) {
                significantEventEmitted = true;
            }
        }
    }
    {
        KeyboardEvent ev;
        while(Events::Keyboard->Read(&ev)) {
            significantEventEmitted = true;
            WindowManager::DispatchEvent(ev);
        }
    }
    {
        MouseEvent ev;
        while(Events::Mouse->Read(&ev)) {
            significantEventEmitted = true;
            Point screenSize(Graphics::ScreenWidth(), Graphics::ScreenHeight());
            bool ok = Cursor::UpdatePosition(ev, screenSize);
            if ( !(ok) ) { continue; }
            ev.pos = Cursor::GetPosition();
            WindowManager::DispatchEvent(ev);
        }
    }
    return significantEventEmitted;
}

void RenderUI() {
    Canvas* canvas = Graphics::GetScreenCanvas();
    WindowManager::RenderAll(*canvas);
    Cursor::Render(*canvas);
    Graphics::FlushScreenCanvas();
}

#endif

