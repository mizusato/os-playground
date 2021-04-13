#include "../boot.h"
#include "types.h"
#include "gfx.hpp"

class ScreenInvalid final: public Screen {
public:
    ScreenInvalid(GraphicsInfo* gfx): Screen(gfx) {};
    ~ScreenInvalid() {}
    void DrawPixel(Number x, Number y, Number r, Number g, Number b) override {};
};

class ScreenRGB final: public Screen {
public:
    ScreenRGB(GraphicsInfo* gfx): Screen(gfx) {};
    ~ScreenRGB() {};
    void DrawPixel(Number x, Number y, Number r, Number g, Number b) override {
        gfx.framebuffer[(y * gfx.screenBufferWidth) + x] = r | (g << 8) | (b << 16);
    };
};

class ScreenBGR final: public Screen {
public:
    ScreenBGR(GraphicsInfo* gfx): Screen(gfx) {};
    ~ScreenBGR() {};
    void DrawPixel(Number x, Number y, Number r, Number g, Number b) override {
        gfx.framebuffer[(y * gfx.screenBufferWidth) + x] = b | (g << 8) | (r << 16);
    };
};

Shared<Screen> Screen::From(GraphicsInfo* gfx) {
    PixelFormat pf = gfx->pixelFormat;
    if (pf == PF_RGB) {
        return Shared<Screen>(new ScreenRGB(gfx));
    } else if (pf == PF_BGR) {
        return Shared<Screen>(new ScreenBGR(gfx));
    } else {
        return Shared<Screen>(new ScreenInvalid(gfx));
    }
}

