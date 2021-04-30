#include "panic.hpp"
#include "window.hpp"

namespace WindowManager {
    List<Window*>* windows;
    Window* activeWindow;
    void Init() {
        if (windows != nullptr) {
            panic("WindowManager::Init(): already initialized");
        }
        windows = new List<Window*>();
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
            auto viewport = canvas.SliceView(current->position, current->size);
            current->Render(*viewport, (current == activeWindow));
        }
    }
    void DispatchEvent(KeyboardEvent ev) {
        if (activeWindow != nullptr) {
            activeWindow->DispatchEvent(ev);
        }
    }
    void DispatchEvent(MouseEvent ev, MouseEvent prev_ev) {
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
                    current->DispatchEvent(ev);
                }
            }
            if (current->Contains(ev.pos)) {
                if (current != activeWindow && ev.down) {
                    Raise(current);
                }
                if (!(current->Contains(prev_ev.pos))) {
                    ev.in = true;
                }
                ev.pos = (ev.pos - current->position);
                current->DispatchEvent(ev);
                break;
            }
        }
    }
};

bool Window::Contains(Point p) const {
    Point pos = this->position;
    Point bound = (pos + this->size);
    return (pos.X <= p.X && p.X < bound.X && pos.Y <= p.Y && p.Y < bound.Y);
}

