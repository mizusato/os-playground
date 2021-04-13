#include "../boot.h"
#include "types.h"
#include "font.hpp"
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

void Screen::DrawString(Number base_x, Number base_y, const char* s) {
    Number w = 18;
    Number h = 36;
    Number i = 0;
    while(*s != 0) {
        char ch = *s;
        Number n = (ch - 'a');
        for (Number dy = 0; dy < h; dy += 1) {
            const Number* row = &BasicFont[h*n+dy];
            for (Number dx = 0; dx < w; dx += 1) {
                Number x = base_x + (i * w) + dx;
                Number y = base_y + dy;
                Number offset = (w - dx - 1);
                bool black = false;
                if ('a' < ch && ch < 'z') {
                    black = (*row & (1 << offset));
                }
                if (black) {
                    this->DrawPixel(x, y, 0, 0, 0);
                } else {
                    this->DrawPixel(x, y, 0xFF, 0xFF, 0xFF);
                }
            }
        }
        i++;
        s++;
    }
}

