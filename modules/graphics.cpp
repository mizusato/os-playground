#include "../boot.h"
#include "font.hpp"
#include "graphics.hpp"


class ScreenRGB final: public Screen {
public:
    ScreenRGB(GraphicsInfo* gfxInfo): Screen(gfxInfo) {};
    ~ScreenRGB() {};
    void DrawPixel(Number x, Number y, Number r, Number g, Number b) override {
        gfxInfo.framebuffer[(y * gfxInfo.screenBufferWidth) + x] = r | (g << 8) | (b << 16);
    };
};

class ScreenBGR final: public Screen {
public:
    ScreenBGR(GraphicsInfo* gfx): Screen(gfx) {};
    ~ScreenBGR() {};
    void DrawPixel(Number x, Number y, Number r, Number g, Number b) override {
        gfxInfo.framebuffer[(y * gfxInfo.screenBufferWidth) + x] = b | (g << 8) | (r << 16);
    };
};

namespace Graphics {
    Screen* screen = nullptr;
    void Init(GraphicsInfo* gfxInfo) {
        if (screen != nullptr) {
            delete screen;
        }
        PixelFormat pf = gfxInfo->pixelFormat;
        if (pf == PF_RGB) {
            screen = new ScreenRGB(gfxInfo);
        } else if (pf == PF_BGR) {
            screen = new ScreenBGR(gfxInfo);
        } else {
            screen = nullptr;
        }
    }
    Number ScreenWidth() {
        return screen->Width();
    }
    Number ScreenHeight() {
        return screen->Height();
    }
    void DrawPixel(Number x, Number y, Number r, Number g, Number b) {
        screen->DrawPixel(x, y, r, g, b);
    }
    void DrawString(Number base_x, Number base_y, String str) {
        Number w = BASIC_FONT_WIDTH;
        Number h = BASIC_FONT_HEIGHT;
        Number i = 0;
        for (auto it = str.Iterate(); it->NotEmpty(); it->Shift()) {
            char ch = it->CurrentChar();
            for (Number dy = 0; dy < h; dy += 1) {
                const Number* data = BasicFont::GetCharData(ch);
                for (Number dx = 0; dx < w; dx += 1) {
                    Number x = base_x + (i * w) + dx;
                    Number y = base_y + dy;
                    Number offset = (w - dx - 1);
                    bool black = false;
                    if (data != nullptr) {
                        black = (data[dy] & (1 << offset));
                    }
                    if (black) {
                        DrawPixel(x, y, 0, 0, 0);
                    } else {
                        DrawPixel(x, y, 0xFF, 0xFF, 0xFF);
                    }
                }
            }
            i++;
        }
    }
};


