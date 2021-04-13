#ifndef GFX_HPP
#define GFX_HPP

#include "shared.hpp"
#include "../boot.h"
#include "types.h"


class Screen {
public:
    virtual void DrawPixel(Number x, Number y, Number r, Number g, Number b) = 0;
    virtual ~Screen() {};
    static Shared<Screen> From(GraphicsInfo* gfx);
    Number Width() { return gfx.screenViewportWidth; };
    Number Height() { return gfx.screenHeight; };
protected:
    Screen(GraphicsInfo* gfx): gfx(*gfx) {};
    GraphicsInfo gfx;
};

#endif
