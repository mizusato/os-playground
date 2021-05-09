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
    struct State {
        Number next_cmd_id = 0;
    };
    Unique<State> state;
    Console(Point pos, Point size, String title, Options opts);
public:
    ~Console();
    void RenderContent(Canvas& target, bool active) override;
    void DispatchContentEvent(KeyboardEvent ev) override;
    void DispatchContentEvent(MouseEvent ev) override;
    void HandleClose() override;
    void ExecuteCommand(String command);
    static void Open(Point pos, Point size, String title, Options opts);
    enum MessageType {
        M_Input,
        M_Output,
        M_Info,
        M_Success,
        M_Failure
    };
    void ShowMessage(String content, MessageType type, Number source_id);
};

#endif
