#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "graphics.hpp"
#include "events.hpp"

class Window {
protected:
    Window(Point position, Point size):
        geometry(Unique<Geometry>(new Geometry(position, size))) {};
public:
    struct Geometry {
        Point position;
        Point size;
        Geometry(Point position, Point size): position(position), size(size) {};
    };
    Unique<Geometry> geometry;
    virtual ~Window() {};
    virtual void Render(Canvas& target, bool active) = 0;
    virtual void DispatchEvent(KeyboardEvent ev) = 0;
    virtual void DispatchEvent(MouseEvent ev) = 0;
    struct Flags {
        bool  background = false; 
    } __attribute__((packed)) flags;
    bool Contains(Point p) const;
};

namespace WindowManager {
    void Init();
    void Add(Window* window);
    void Remove(Window* window);
    void Raise(Window* window);
    void RenderAll(Canvas& canvas);
    void DispatchEvent(KeyboardEvent ev);
    void DispatchEvent(MouseEvent ev);
};

#endif

