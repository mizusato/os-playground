#ifndef UI_CONSOLE_HPP
#define UI_CONSOLE_HPP

#include "windows.hpp"
#include "widgets.hpp"

class Console final: public BaseWindow {
private:
    struct Widgets {
        Unique<LinearLayout> layout;
        Unique<TextDisplay> display;
        Unique<LineEdit> edit;
        const Widget* focus;
        Widgets();
    };
    Unique<Widgets> widgets;
    Console(Point pos, Point size, String title, Options opts);
public:
    ~Console();
    void RenderContent(Canvas& target, bool active) override;
    void DispatchContentEvent(KeyboardEvent ev) override;
    void DispatchContentEvent(MouseEvent ev) override;
    void HandleClose() override;
    static void Open(Point pos, Point size, String title, Options opts);
};

#endif
