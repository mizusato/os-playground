#ifndef GFX_HPP
#define GFX_HPP

#include "../boot.h"
#include "types.h"


class Screen {
public:
    static void Init(GraphicsInfo* gfx);
    static Screen* Instance() { return instance; };
    virtual ~Screen() {};
    virtual void DrawPixel(Number x, Number y, Number r, Number g, Number b) = 0;
    Number Width()  { return gfx.screenViewportWidth; };
    Number Height() { return gfx.screenHeight; };
    void DrawString(Number x, Number y, const char* s);
protected:
    Screen(GraphicsInfo* gfx): gfx(*gfx) {};
    GraphicsInfo gfx;
    static Screen* instance;
};

#endif
