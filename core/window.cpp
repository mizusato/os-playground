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
};

