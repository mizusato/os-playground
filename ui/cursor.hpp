#ifndef UI_CURSOR_HPP
#define UI_CURSOR_HPP

#include "../core/graphics.hpp"
#include "../core/events.hpp"

namespace Cursor {
    bool UpdatePosition(Point* pos, const MouseEvent& ev, const Point& screen_size) {
        Number x = pos->X;
        Number y = pos->Y;
        Number new_x = (x + ev.pos.X);
        Number new_y = (y + ev.pos.Y);
        if (
            (new_x >= screen_size.X)
            || (new_y >= screen_size.Y)
            || ((new_x > x) && ((new_x - x) >= screen_size.X))
            || ((x > new_x) && ((x - new_x) >= screen_size.X))
            || ((new_y > y) && ((new_y - y) >= screen_size.Y))
            || ((y > new_y) && ((y - new_y) >= screen_size.Y))
        ) {
            return false;
        } else {
            pos->X = new_x;
            pos->Y = new_y;
            return true;
        }
    }
    void Render(const Point& pos, Canvas& canvas) {
        for (Number dy = 0; dy < 36; dy += 1) {
            for (Number dx = 0; dx < 18; dx += 1) {
                Number x = pos.X + dx;
                Number y = pos.Y + dy;
                canvas.DrawPixel(x, y, 0x33, 0xFF, 0xA0, 0xFF);
            }
        }
    }
};

#endif
