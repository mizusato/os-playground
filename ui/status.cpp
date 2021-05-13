#include "../core/scheduler.hpp"
#include "status.hpp"

StatusWindow::State::State():
    text(""), display(Unique<TextDisplay>(new TextDisplay(&focus))) {}

StatusWindow::StatusWindow(Point pos, Point size, String title, Options opts):
    BaseWindow(pos, size, title, opts),
    state(new State) {
        WindowManager::Add(this);
    }

StatusWindow::~StatusWindow() {
    WindowManager::Remove(this);
}

void StatusWindow::RenderContent(Canvas& target, bool active) {
    auto& display = state->display;
    display->Render(target, active);
}

void StatusWindow::DispatchContentEvent(KeyboardEvent ev) {
    if (ev.ctrl) {
        if (ev.key == 'p') { state->display->Scroll(TextDisplay::Up); } else
        if (ev.key == 'n') { state->display->Scroll(TextDisplay::Down); } else
        if (ev.key == 'f') { state->display->Scroll(TextDisplay::Right); } else
        if (ev.key == 'b') { state->display->Scroll(TextDisplay::Left); }
    }
}

void StatusWindow::SetText(String content) {
    state->text = content;
    auto& display = state->display;
    Color color(0, 0, 0, 0xFF);
    display->Clear();
    display->Add(state->text, TextStyle(color));
}


TaskStatusWindow::TaskStatusWindow(Point pos, Point size, String title, Options opts):
    StatusWindow(pos, size, title, opts) {}

void TaskStatusWindow::Destroy() {
    delete this;
}

void TaskStatusWindow::SetTitle(String title) {
    ChangeTitle(title);
}

void TaskStatusWindow::SetContent(String content) {
    SetText(content);
}

void TaskStatusWindow::HandleClose() {
    Scheduler::GetInstance()->Kill(this);
}


MemoryMonitor::MemoryMonitor(Point pos, Options opts):
    StatusWindow(pos, Point(450, 90), "Memory Monitor", opts) {}

void MemoryMonitor::Update(HeapStatus status) {
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


