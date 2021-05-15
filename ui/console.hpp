#ifndef UI_CONSOLE_HPP
#define UI_CONSOLE_HPP

#include "windows.hpp"
#include "widgets.hpp"
#include "../shell/runtime.hpp"

class Console final: public BaseWindow, public AbstractConsole {
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
        Number nextCommand;
        struct CommandMapping {
            Number task;
            Number cmd;
        };
        Unique<List<CommandMapping>> runningCommands;
        State();
    };
    Unique<State> state;
    Console(Point pos, Point size, String title, Options opts);
    static Number nextConsoleId;
    static String GetNextTitle();
    void Dispose();
public:
    ~Console();
    void RenderContent(Canvas& target, bool active) override;
    void DispatchContentEvent(KeyboardEvent ev) override;
    void DispatchContentEvent(MouseEvent ev) override;
    void HandleClose() override;
    void ExecuteCommand(String command);
    static void Open(Point pos, Point size, Options opts);
    enum MessageType {
        M_Input,
        M_Output,
        M_Info,
        M_Success,
        M_Failure
    };
    void ShowMessage(String content, MessageType type, Number cmd);
    void Start(Shared<Program> p, Number cmd);
    bool LookupTaskId(Number cmd, Number* taskId);
    void TaskNotify(String content, MessageType type, Number cmd);
    void TaskNotifyStarted(Number cmd, Number taskId) override;
    void TaskNotifyKilled(Number cmd) override;
    void TaskNotifyDone(Number cmd, bool ok, String msg) override;
    void TaskPrintLog(Number cmd, String msg) override;
};

#endif
