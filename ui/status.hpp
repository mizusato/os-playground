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
    void DispatchContentEvent(KeyboardEvent ev);
    void DispatchContentEvent(MouseEvent ev);
    void HandleClose() override;
};

class MemoryMonitor final: public StatusWindow {
public:
    MemoryMonitor(Point pos, Options opts);
    ~MemoryMonitor() {};
    void Update(HeapStatus status);
};

#endif


