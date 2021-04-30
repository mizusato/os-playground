#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "graphics.hpp"
#include "events.hpp"

class Window {
protected:
    Window(): position(Point(0,0)), size(Point(300,300)) {};
    Window(Point position, Point size): position(position), size(size) {};
public:
    Point position;
    Point size;
    virtual ~Window() {};
    virtual void Render(Canvas& target, bool active) = 0;
    virtual void DispatchEvent(KeyboardEvent ev) = 0;
    virtual void DispatchEvent(MouseEvent ev) = 0;
    struct Flags {
        bool  background = false; 
    } flags;
    bool Contains(Point p) const;
};

namespace WindowManager {
    void Init();
    void Add(Window* window);
    void Remove(Window* window);
    void Raise(Window* window);
    void RenderAll(Canvas& canvas);
    void DispatchEvent(KeyboardEvent ev);
    void DispatchEvent(MouseEvent ev, MouseEvent prev_ev);
};

#endif

