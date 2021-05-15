#include "panic.hpp"
#include "window.hpp"

namespace WindowManager {
    List<Window*>* windows;
    Window* activeWindow;
    MouseEvent prev_mouse_ev;
    void Init() {
        if (windows != nullptr) {
            panic("window-manager: already initialized");
        }
        windows = new List<Window*>();
        prev_mouse_ev = MouseEvent();
        prev_mouse_ev.pos = Point(0xbadbeef, 0xbadbeef);
    }
    void Add(Window* w) {
        windows->Append(w);
        activeWindow = w;
    }
    void Remove(Window* w) {
        auto draft = new List<Window*>();
        activeWindow = nullptr;
        for (auto it = windows->Iterate(); it->HasCurrent(); it->Proceed()) {
            auto current = it->Current();
            if (current != w) {
                draft->Append(current);
                activeWindow = current;
            }
        }
        delete windows;
        windows = draft;
    }
    void Raise(Window* w) {
        if (w->flags.background) { return; }
        auto draft = new List<Window*>();
        bool found = false;
        for (auto it = windows->Iterate(); it->HasCurrent(); it->Proceed()) {
            auto current = it->Current();
            if (current == w) {
                found = true;
            } else {
                draft->Append(current);
                activeWindow = current;
            }
        }
        if (found) {
            draft->Append(w);
            activeWindow = w;
        }
        delete windows;
        windows = draft;
    }
    void RenderAll(Canvas& canvas) {
        for (auto it = windows->Iterate(); it->HasCurrent(); it->Proceed()) {
            auto current = it->Current();
            auto& g = current->geometry;
            auto viewport = canvas.SliceView(g->position, g->size);
            current->Render(*viewport, (current == activeWindow));
        }
    }
    void DispatchEvent(KeyboardEvent ev) {
        if (activeWindow != nullptr) {
            activeWindow->DispatchEvent(ev);
        }
    }
    void DispatchEvent(MouseEvent ev) {
        MouseEvent prev_ev = prev_mouse_ev;
        prev_mouse_ev = ev;
        if (!(prev_ev.btnLeft) && ev.btnLeft) {
            ev.down = true;
        }
        if (prev_ev.btnLeft && !(ev.btnLeft)) {
            ev.up = true;
        }
        auto reversed = Unique<List<Window*>>(new List<Window*>());
        for (auto it = windows->Iterate(); it->HasCurrent(); it->Proceed()) {
            reversed->Prepend(it->Current());
        }
        for (auto it = reversed->Iterate(); it->HasCurrent(); it->Proceed()) {
            Window* current = it->Current();
            if (current->Contains(prev_ev.pos)) {
                if (!(current->Contains(ev.pos))) {
                    MouseEvent out_ev;
                    out_ev.pos = prev_ev.pos;
                    out_ev.out = true;
                    current->DispatchEvent(out_ev);
                }
            }
            if (current->Contains(ev.pos)) {
                if (current != activeWindow && ev.down) {
                    Raise(current);
                }
                if (!(current->Contains(prev_ev.pos))) {
                    ev.in = true;
                }
                ev.pos = (ev.pos - current->geometry->position);
                current->DispatchEvent(ev);
                break;
            }
        }
    }
};

bool Window::Contains(Point p) const {
    return InArea(p, geometry->position, geometry->size);
}

