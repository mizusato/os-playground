#ifndef GFX_HPP
#define GFX_HPP

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

class Screen {
public:
    virtual ~Screen() {};
    virtual void DrawPixel(Number x, Number y, Number r, Number g, Number b) = 0;
    Number Width()  { return gfxInfo.screenViewportWidth; };
    Number Height() { return gfxInfo.screenHeight; };
protected:
    Screen(GraphicsInfo* gfxInfo): gfxInfo(*gfxInfo) {};
    GraphicsInfo gfxInfo;
};

#endif
