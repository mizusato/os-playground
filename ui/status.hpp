#ifndef UI_STATUS_HPP
#define UI_STATUS_HPP

#include "windows.hpp"
#include "widgets.hpp"
#include "../shell/runtime.hpp"


class StatusWindow: public BaseWindow {
private:
    struct State {
        String text;
        Unique<TextDisplay> display;
        const Widget* focus = nullptr;
        State();
    };
    Unique<State> state;
public:
    StatusWindow(Point pos, Point size, String title, Options opts);
    virtual ~StatusWindow();
    void SetText(String newText);
    void RenderContent(Canvas& target, bool active) override;
    void DispatchContentEvent(KeyboardEvent ev) override;
};

class TaskStatusWindow final: public StatusWindow, public AbstractWindow {
public:
    TaskStatusWindow(Point pos, Point size, String title, Options opts);
    ~TaskStatusWindow() {};
    void Destroy() override;
    void SetTitle(String title) override;
    void SetContent(String content) override;
    void HandleClose() override;
};

class MemoryMonitor final: public StatusWindow {
public:
    MemoryMonitor(Point pos, Options opts);
    ~MemoryMonitor() {};
    void Update(HeapStatus status);
};

/*
class TimerInspector final: public StatusWindow {
public:
    TimerInspector(Point pos, Options opts):
        StatusWindow(pos, Point(100, 60), "Timer", opts) {};
    ~TimerInspector() {};
    void Update(const TimerEvent& ev) {
        // 1000ms (1s)
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
*/

#endif

