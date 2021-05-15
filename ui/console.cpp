#include "../core/info.hpp"
#include "../core/scheduler.hpp"
#include "../userland/all.hpp"
#include "status.hpp"
#include "console.hpp"


Console::Widgets::Widgets():
    layout(Unique<LinearLayout>(new LinearLayout(&focus, LinearLayout::Vertical))),
    display(Unique<TextDisplay>(new TextDisplay(&focus))),
    edit(Unique<LineEdit>(new LineEdit(&focus))) {
        focus = edit.get();
        display->Add("* Interactive Shell *\n\n", TextStyle(Color(0, 0, 0, 0xFF)));
        layout->Append(display.get(), 1);
        layout->Append(edit.get(), 0);
    }

Console::State::State():
    nextCommand(0),
    runningCommands(Unique<List<CommandMapping>>(new List<CommandMapping>())) {}

Console::Console(Point pos, Point size, String title, Options opts):
    BaseWindow(pos, size, title, opts),
    widgets(Unique<Widgets>(new Widgets)),
    state(Unique<State>(new State)) {
        WindowManager::Add(this);
    }

Console::~Console() {
    WindowManager::Remove(this);
}

Number Console::nextConsoleId = 0;

String Console::GetNextTitle() {
    String::Builder buf;
    buf.Write("Console ");
    buf.Write(String(nextConsoleId));
    nextConsoleId += 1;
    return buf.Collect();
}

void Console::Dispose() {
    for (auto it = state->runningCommands->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto mapping = it->Current();
        Scheduler::GetInstance()->Kill(mapping.task, true);
    }
}

void Console::Open(Point pos, Point size, Options opts) {
    String title = GetNextTitle();
    new Console(pos, size, title, opts);
}

void Console::HandleClose() {
    Dispose();
    delete this;
}

void Console::RenderContent(Canvas& target, bool active) {
    widgets->layout->Render(target, active);
}

void Console::DispatchContentEvent(KeyboardEvent ev) {
    if (ev.key == 0xD) {
        String cmd = widgets->edit->CurrentText();
        widgets->edit->Clear();
        ExecuteCommand(cmd);
    } else {
        if (ev.ctrl) {
            if (ev.key == 'p') { widgets->display->Scroll(TextDisplay::Up); } else
            if (ev.key == 'n') { widgets->display->Scroll(TextDisplay::Down); } else
            if (ev.key == 'f') { widgets->display->Scroll(TextDisplay::Right); } else
            if (ev.key == 'b') { widgets->display->Scroll(TextDisplay::Left); }
        }
        widgets->layout->DispatchEvent(ev);
    }
}

void Console::DispatchContentEvent(MouseEvent ev) {
    widgets->layout->DispatchEvent(ev);
}

void Console::ExecuteCommand(String command) {
    Number cmd = state->nextCommand;
    state->nextCommand += 1;
    ShowMessage(command, M_Input, cmd);
    if (command == "help") {
        ShowMessage (
            "<scroll>\n"
            "C-p C-n C-b C-f\n"
            "<commands>\n"
            "help, cmd, meminfo, countdown,\n"
            "ticker, keyboard, mouse",
            M_Success, cmd
        );
    } else if (command == "cmd") {
        Options new_opts = *opts;
        new_opts.closable = true;
        Point new_pos = (geometry->position + Point(50, 50));
        Point new_size = geometry->size;
        Open(new_pos, new_size, new_opts);
        ShowMessage("new console opened", M_Success, cmd);
    } else if (command == "meminfo") {
        ShowMessage(GetMemoryInfo(), M_Output, cmd);
        ShowMessage("exited", M_Success, cmd);
    } else if (command == "countdown") {
        auto p = Shared<Program>(new Userland::Countdown(32768));
        Start(p, cmd);
    } else if (command == "ticker") {
        auto p = Shared<Program>(new Userland::Ticker());
        Start(p, cmd);
    } else if (command == "keyboard") {
        auto p = Shared<Program>(new Userland::KeyboardInspector());
        Start(p, cmd);
    } else if (command == "mouse") {
        auto p = Shared<Program>(new Userland::MouseInspector());
        Start(p, cmd);
    } else {
        ShowMessage("invalid command\n" "use 'help' to get usage info", M_Failure, cmd);
    }
}

void Console::ShowMessage(String content, MessageType type, Number cmd) {
    String type_desc =
        (type == M_Input)? "Input":
        (type == M_Output)? "Output":
        (type == M_Info)? "Info":
        (type == M_Success)? "Success":
        (type == M_Failure)? "Failure": "";
    Color type_color = 
        (type == M_Input)? Color(0x55, 0x55, 0x55, 0xFF):
        (type == M_Output)? Color(0x55, 0x55, 0x55, 0xFF):
        (type == M_Info)? Color(0x33, 0x33, 0xFF, 0xFF):
        (type == M_Success)? Color(0x33, 0xCD, 0x33, 0xFF):
        (type == M_Failure)? Color(0xFF, 0x33, 0x33, 0xFF): Color(0, 0, 0, 0xFF);
    String::Builder buf;
    buf.Write("["); buf.Write(String(cmd)); buf.Write("] ");
    buf.Write(type_desc);
    String title = buf.Collect();
    TextStyle title_style = TextStyle(type_color);
    widgets->display->Add(title, title_style);
    TextStyle content_style = TextStyle(Color(0, 0, 0, 0xFF));
    widgets->display->Add(content, content_style);
    widgets->display->Add("\n", content_style);
}

void Console::Start(Shared<Program> p, Number cmd) {
    Point pos = (geometry->position + Point(50, 50));
    AbstractWindow* w = new TaskStatusWindow(pos, Point(300, 200), "task", Options());
    AbstractConsole* c = this;
    auto ctx = Unique<TaskContext>(new TaskContext);
    ctx->window = w;
    ctx->console = c;
    ctx->consoleCommandNumber = cmd;
    Scheduler::GetInstance()->Start(p, std::move(ctx));
}

bool Console::LookupTaskId(Number cmd, Number* task) {
    for (auto it = state->runningCommands->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto mapping = it->Current();
        if (mapping.cmd == cmd) {
            *task = mapping.task;
            return true;
        }
    }
    return false;
}

void Console::TaskNotify(String content, MessageType type, Number cmd) {
    String::Builder buf;
    buf.Write("task"); 
    Number task;
    if (LookupTaskId(cmd, &task)) {
        buf.Write(" "); buf.Write(String(task));
    }
    buf.Write(": ");
    buf.Write(content);
    String msg = buf.Collect();
    ShowMessage(msg, type, cmd);
}

void Console::TaskNotifyStarted(Number cmd, Number task) {
    State::CommandMapping mapping;
    mapping.task = task;
    mapping.cmd = cmd;
    state->runningCommands->Append(mapping);
    TaskNotify("started", M_Info, cmd);
}

void Console::TaskNotifyKilled(Number cmd) {
    TaskNotify("killed", M_Failure, cmd);
}

void Console::TaskNotifyDone(Number cmd, bool ok, String msg) {
    String::Builder buf;
    buf.Write(ok? "done": "failed");
    buf.Write("\n");
    buf.Write(msg);
    String wrapped_msg = buf.Collect();
    TaskNotify(wrapped_msg, (ok? M_Success: M_Failure), cmd);
}

void Console::TaskPrintLog(Number cmd, String msg) {
    TaskNotify(msg, M_Output, cmd);
}


