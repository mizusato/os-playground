#ifndef UI_STATUS_HPP
#define UI_STATUS_HPP

#include "windows.hpp"
#include "widgets.hpp"


class StatusWindow: public BaseWindow {
private:
    struct State {
        String text;
        Unique<TextDisplay> display;
        const Widget* focus = nullptr;
        State(): text(""), display(Unique<TextDisplay>(new TextDisplay(&focus))) {}
    };
    Unique<State> state;
protected:
    StatusWindow(Point pos, Point size, String title, Options opts):
        BaseWindow(pos, size, title, opts),
        state(new State) {
            WindowManager::Add(this);
        };
public:
    virtual ~StatusWindow() {
        WindowManager::Remove(this);
    };
    void SetText(String new_text) {
        state->text = new_text;
        auto& display = state->display;
        Color color(0, 0, 0, 0xFF);
        display->Clear();
        display->Add(state->text, TextStyle(color));
    }
    void RenderContent(Canvas& target, bool active) override {
        auto& display = state->display;
        display->Render(target, active);
    }
    void DispatchContentEvent(KeyboardEvent ev) override {
        if (ev.ctrl) {
            if (ev.key == 'p') { state->display->Scroll(TextDisplay::Up); } else
            if (ev.key == 'n') { state->display->Scroll(TextDisplay::Down); } else
            if (ev.key == 'f') { state->display->Scroll(TextDisplay::Right); } else
            if (ev.key == 'b') { state->display->Scroll(TextDisplay::Left); }
        }
    }
};

class MemoryMonitor final: public StatusWindow {
public:
    MemoryMonitor(Point pos, Options opts):
        StatusWindow(pos, Point(450, 90), "Memory Monitor", opts) {};
    ~MemoryMonitor() {};
    void Update(HeapStatus status) {
        String::Builder buf;
        Number used = (status.ChunksTotal - status.ChunksAvailable);
        buf.Write("chunks: ");
        buf.Write(String(used));
        buf.Write("/");
        buf.Write(String(status.ChunksTotal));
        buf.Write("\n");
        buf.Write("size: (");
        buf.Write(String::ReadableSize(used * sizeof(Chunk)));
        buf.Write(")/(");
        buf.Write(String::ReadableSize(status.ChunksTotal * sizeof(Chunk)));
        buf.Write(")");
        SetText(buf.Collect());
    }
};

class TimerInspector final: public StatusWindow {
public:
    TimerInspector(Point pos, Options opts):
        StatusWindow(pos, Point(100, 60), "Timer", opts) {};
    ~TimerInspector() {};
    void Update(const TimerEvent& ev) {
        if ((ev.count % 10) == 0) {
            SetText(String(ev.count / 10));
        }
    }
};

class KeyboardInspector final: public StatusWindow {
public:
    KeyboardInspector(Point pos, Options opts):
        StatusWindow(pos, Point(200, 100), "Keyboard", opts) {};
    ~KeyboardInspector() {};
    void Update(const KeyboardEvent& ev) {
        String::Builder buf;
        buf.Write(String::Chr(ev.key));
        buf.Write(" ");
        buf.Write(String::Hex(ev.key));
        if (ev.shift) { buf.Write(" + shift"); }
        buf.Write("\n");
        if (ev.ctrl) { buf.Write("[ctrl] "); }
        if (ev.alt) { buf.Write("[alt] "); }
        SetText(buf.Collect());
    }
};

class MouseInspector final: public StatusWindow {
public:
    MouseInspector(Point pos, Options opts):
        StatusWindow(pos, Point(240, 180), "Mouse", opts) {};
    void Update(const MouseEvent& ev) {
        String::Builder buf;
        buf.Write("mouse:");
        buf.Write("\n");
        buf.Write("x: ");
        buf.Write(String(ev.pos.X));
        buf.Write("\n");
        buf.Write("y: ");
        buf.Write(String(ev.pos.Y));
        buf.Write("\n");
        buf.Write("left: ");
        buf.Write(String(ev.btnLeft));
        buf.Write("\n");
        buf.Write("alt: ");
        buf.Write(String(ev.alt));
        SetText(buf.Collect());
    }
};

#endif

