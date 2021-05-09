#include "../core/info.hpp"
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

Console::Console(Point pos, Point size, String title, Options opts):
    BaseWindow(pos, size, title, opts),
    widgets(Unique<Widgets>(new Widgets)),
    state(Unique<State>(new State)) {
        WindowManager::Add(this);
    }

Console::~Console() {
    WindowManager::Remove(this);
}

void Console::Open(Point pos, Point size, String title, Options opts) {
    new Console(pos, size, title, opts);
}

void Console::HandleClose() {
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
    Number id = state->next_cmd_id;
    state->next_cmd_id += 1;
    ShowMessage(command, M_Input, id);
    if (command == "help") {
        ShowMessage (
            "scroll: C-p C-n C-b C-f\n"
            "commands: help, meminfo",
            M_Success, id
        );
    } else if (command == "meminfo") {
        ShowMessage(GetMemoryInfo(), M_Output, id);
        ShowMessage("exited", M_Success, id);
    } else {
        ShowMessage("invalid command\n" "use 'help' to get usage info", M_Failure, id);
    }
}

void Console::ShowMessage(String content, MessageType type, Number source_id) {
    String type_desc =
        (type == M_Input)? "Input":
        (type == M_Output)? "Output":
        (type == M_Info)? "Info":
        (type == M_Success)? "OK":
        (type == M_Failure)? "Failed": "";
    Color type_color = 
        (type == M_Input)? Color(0x55, 0x55, 0x55, 0xFF):
        (type == M_Output)? Color(0x55, 0x55, 0x55, 0xFF):
        (type == M_Info)? Color(0x33, 0x33, 0xFF, 0xFF):
        (type == M_Success)? Color(0x33, 0xCD, 0x33, 0xFF):
        (type == M_Failure)? Color(0xFF, 0x33, 0x33, 0xFF): Color(0, 0, 0, 0xFF);
    String::Builder buf;
    buf.Write("[");
    buf.Write(String(source_id));
    buf.Write("] ");
    buf.Write(type_desc);
    String title = buf.Collect();
    TextStyle title_style = TextStyle(type_color);
    widgets->display->Add(title, title_style);
    TextStyle content_style = TextStyle(Color(0, 0, 0, 0xFF));
    widgets->display->Add(content, content_style);
    widgets->display->Add("\n", content_style);
}

