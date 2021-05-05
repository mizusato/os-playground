#ifndef UI_LOG_HPP
#define UI_LOG_HPP

#include "../core/log.hpp"
#include "windows.hpp"
#include "widgets.hpp"

class LogViewer final: public BaseWindow {
private:
    const Widget* focus;
    Unique<TextDisplay> display;
    LogViewer(Point pos, Point size, String title, Options opts);
public:
    ~LogViewer() {};
    void RenderContent(Canvas& target, bool active) override;
    void Log(String content, LogLevel level);
    static LogViewer* instance;
    static void Open(Point pos, Point size, String title, Options opts);
};

#endif

