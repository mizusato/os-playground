#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "graphics.hpp"

class Window {
protected:
    Window(): position(Point(0,0)), size(Point(300,300)) {};
public:
    Point position;
    Point size;
    virtual ~Window() {};
    virtual void Render(Canvas& target, bool active) = 0;
    // additional options used by WindowManager
    struct Flags {
        bool  background; 
    } flags;
};

namespace WindowManager {
    void Init();
    void Add(Window* window);
    void Remove(Window* window);
    void Raise(Window* window);
    void RenderAll(Canvas& canvas);
};

#endif

