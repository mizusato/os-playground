#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../boot.h"
#include "types.h"
#include "string.hpp"


namespace Graphics {
    void Init(GraphicsInfo* gfxInfo);
    Number ScreenWidth();
    Number ScreenHeight();
    void DrawPixel(Number x, Number y, Number r, Number g, Number b);
    void DrawString(Number base_x, Number base_y, const String& str);
};

#endif

