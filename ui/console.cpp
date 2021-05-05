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
    widgets(Unique<Widgets>(new Widgets)) {
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
    widgets->layout->DispatchEvent(ev);
}

void Console::DispatchContentEvent(MouseEvent ev) {
    widgets->layout->DispatchEvent(ev);
}

