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
    struct State {
        String  title;
        bool    dragging;
        Point   dragPoint;
        State();
    };
protected:
    Unique<Options> opts;
    Unique<State> state;
    BaseWindow(Point pos, Point size, String title, Options opts);
public:
    virtual ~BaseWindow() {};
    void GetContentArea(Point* start, Point* span);
    void Render(Canvas& target, bool active) override;
    void DispatchEvent(KeyboardEvent ev) override;
    void DispatchEvent(MouseEvent ev) override;
    virtual void RenderContent(Canvas& target, bool active) {};
    virtual void DispatchContentEvent(KeyboardEvent ev) {};
    virtual void DispatchContentEvent(MouseEvent ev) {};
    virtual void HandleClose() {};
};

#endif
