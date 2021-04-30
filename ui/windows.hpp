#ifndef UI_WINDOWS_HPP
#define UI_WINDOWS_HPP

#include "../core/window.hpp"

class BackgroundWindow final: public Window {
private:
    Color color;
public:
    BackgroundWindow(Color color, Point size);
    ~BackgroundWindow() {};
    void Render(Canvas& target, bool active);
    void DispatchEvent(KeyboardEvent ev) {};
    void DispatchEvent(MouseEvent ev) {};
};

class BaseWindow: public Window {
public:
    struct Options {
        bool    closable;
        Number  borderSize;
        Number  titleHeight;
        Color   borderColor;
        Color   titleColorInactive;
        Color   titleColorActive;
        Color   titleFontColor;
        Color   contentColor;
        Options();
    };
protected:
    Unique<Options> opts;
    String title;
    BaseWindow(Point pos, Point size, String title, Options opts);
public:
    virtual ~BaseWindow() {};
    void GetContentArea(Point* start, Point* span);
    void Render(Canvas& target, bool active);
    void DispatchEvent(KeyboardEvent ev);
    void DispatchEvent(MouseEvent ev);
    virtual void RenderContent(Canvas& target, bool active) {};
    virtual void DispatchContentEvent(KeyboardEvent ev) {};
    virtual void DispatchContentEvent(MouseEvent ev) {};
    virtual void HandleClose() {};
};

#endif
