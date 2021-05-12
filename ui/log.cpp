#include "log.hpp"


LogViewer::LogViewer(Point pos, Point size, String title, Options opts):
    BaseWindow(pos, size, title, opts),
    focus(nullptr),
    display(Unique<TextDisplay>(new TextDisplay(&focus))) {}

void LogViewer::RenderContent(Canvas& target, bool active) {
    display->Render(target, active);
}

void LogViewer::Log(String content, LogLevel level) {
    Color color(0, 0, 0, 0xFF);
    if (level == LL_Warning) { color = Color(0xFF, 0xA3, 0x1F, 0xFF); } else
    if (level == LL_Error) { color = Color(0xFF, 0x1F, 0x1F, 0xFF); }
    display->Add(content, TextStyle(color));
}

LogViewer* LogViewer::instance = nullptr;

void LogViewer::Open(Point pos, Point size, String title, Options opts) {
    if (instance == nullptr) {
        instance = new LogViewer(pos, size, title, opts);
        WindowManager::Add(instance);
    } else {
        panic("LogViewer: already opened");
    }
}

void log(String content, LogLevel level) {
    if (LogViewer::instance != nullptr) {
        LogViewer::instance->Log(content, level);
    }
}

