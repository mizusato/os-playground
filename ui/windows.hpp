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
protected:
    BaseWindow();
public:
    virtual ~BaseWindow() {};
};

#endif
