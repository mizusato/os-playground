#include "../boot.h"
#include "font.hpp"
#include "graphics.hpp"


Byte ApplyAlpha(Byte fg_, Byte bg_, Byte alpha_) {
    double fg = (double)(fg_);
    double bg = (double)(bg_);
    double alpha = ((double)(alpha_) / 255);
    Number raw = (Number)(bg + (fg - bg) * alpha); // `round()` omitted
    if (raw < 0) { raw = 0; }
    if (raw > 255) { raw = 255; }
    Byte clamped = (Byte)(raw);
    return clamped;
}

class PixelFormatHandler {
protected:
    PixelFormatHandler() {};
public:
    virtual ~PixelFormatHandler() {};
    virtual Dword MakePixel(Byte r, Byte g, Byte b) = 0;
    virtual void ReadPixel(Dword p, Byte* r, Byte* g, Byte* b) = 0;
};
class PFH_RGB final: public PixelFormatHandler {
    Dword MakePixel(Byte r, Byte g, Byte b) override {
        return (r | (g << 8) | (b << 16));
    }
    void ReadPixel(Dword p, Byte* r, Byte* g, Byte* b) override {
        *r = (p & 0xFF); *g = (p & 0xFF00); *b = (p & 0xFF0000);
    }
};
class PFH_BGR final: public PixelFormatHandler {
    Dword MakePixel(Byte r, Byte g, Byte b) override {
        return (b | (g << 8) | (r << 16));
    }
    void ReadPixel(Dword p, Byte* r, Byte* g, Byte* b) override {
        *r = (p & 0xFF0000); *g = (p & 0xFF00); *b = (p & 0xFF);
    }
};
class PFH_Unsupported: public PixelFormatHandler {
    Dword MakePixel(Byte,Byte,Byte) override {
        return 0;
    }
    void ReadPixel(Dword, Byte* r, Byte* g, Byte* b) override {
        *r = 0; *g = 0; *b = 0;
    }
};

class Screen {
protected:
    GraphicsInfo gfxInfo;
    PixelFormatHandler* pfh;
    Dword* draftBuffer;
    bool*  draftRowDirty;
    bool   draftDirty;
    void initPixelFormatHandler() {
        PixelFormat pf = gfxInfo.pixelFormat;
        if (pf == PF_RGB) { pfh = new PFH_RGB; } else
        if (pf == PF_BGR) { pfh = new PFH_BGR; } else
                          { pfh = new PFH_Unsupported; }
    }
    void initDraftBuffer() {
        Number numberOfPixels = (gfxInfo.screenHeight * gfxInfo.screenBufferWidth);
        void* area = Heap::RequestStatic(numberOfPixels * sizeof(Dword));
        draftBuffer = reinterpret_cast<Dword*>(area);
        for (Number i = 0; i < numberOfPixels; i += 1) {
            draftBuffer[i] = 0;
        }
        area = Heap::RequestStatic(gfxInfo.screenHeight * sizeof(bool));
        draftRowDirty = reinterpret_cast<bool*>(area);
        for (Number i = 0; i < gfxInfo.screenHeight; i += 1) {
            draftRowDirty[i] = false;
        }
    }
public:
    Screen(GraphicsInfo* gfxInfo): gfxInfo(*gfxInfo) {
        initPixelFormatHandler();
        initDraftBuffer();
    };
    virtual ~Screen() {
        delete pfh;
    };
    Number Width()  const { return gfxInfo.screenViewportWidth; };
    Number Height() const { return gfxInfo.screenHeight; };
    void DirectDrawPixel(Number x, Number y, Number r, Number g, Number b) {
        Dword pixel = pfh->MakePixel(r, g, b);
        gfxInfo.framebuffer[x + (y * gfxInfo.screenBufferWidth)] = pixel;
    }
    void DraftSetPixel(Number x, Number y, Number r, Number g, Number b, Number a) {
        if (a == 0) {
            return;
        }
        Dword pixel = pfh->MakePixel(r, g, b);
        Dword* currentPixel = &draftBuffer[x + (y * gfxInfo.screenBufferWidth)];
        if (pixel != *currentPixel) {
            draftRowDirty[y] = true;
            draftDirty = true;
            if (a == 0xFF) {
                *currentPixel = pixel;
            } else {
                Byte cr, cg, cb;
                pfh->ReadPixel(*currentPixel, &cr, &cg, &cb);
                r = ApplyAlpha(r, cr, a);
                g = ApplyAlpha(g, cg, a);
                b = ApplyAlpha(b, cb, a);
                *currentPixel = pfh->MakePixel(r, g, b);
            }
        }
    }
    void DraftCommit() {
        if (draftDirty) {
            draftDirty = false;
            for (Number y = 0; y < gfxInfo.screenHeight; y += 1) {
                if (draftRowDirty[y]) {
                    draftRowDirty[y] = false;
                    for (Number x = 0; x < gfxInfo.screenViewportWidth; x += 1) {
                        Number offset = (x + (y * gfxInfo.screenBufferWidth));
                        gfxInfo.framebuffer[offset] = draftBuffer[offset];
                    }
                }
            }
        }
    }
};

class ScreenRawCanvas final: public Canvas {
private:
    Screen* screen;
public:
    ScreenRawCanvas(Screen* screen): screen(screen) {};
    ~ScreenRawCanvas() {};
    Number Width() const override { return screen->Width(); }
    Number Height() const override { return screen->Height(); }
    void DrawPixel(Number x, Number y, Number r, Number g, Number b, Number _) override {
        screen->DirectDrawPixel(x, y, r, g, b);
    }
};

class ScreenDraftCanvas final: public Canvas {
private:
    Screen* screen;
public:
    ScreenDraftCanvas(Screen* screen): screen(screen) {};
    ~ScreenDraftCanvas() {};
    Number Width() const override { return screen->Width(); }
    Number Height() const override { return screen->Height(); }
    void DrawPixel(Number x, Number y, Number r, Number g, Number b, Number a) override {
        screen->DraftSetPixel(x, y, r, g, b, a);
    }
};

namespace Graphics {
    Screen* screen = nullptr;
    Canvas* screen_raw_canvas;
    Canvas* screen_draft_canvas;
    void Init(GraphicsInfo* gfxInfo) {
        if (screen != nullptr) {
            panic("Graphics::Init(): already initialized");
        }
        screen = new Screen(gfxInfo);
        screen_raw_canvas = new ScreenRawCanvas(screen);
        screen_draft_canvas = new ScreenDraftCanvas(screen);
    }
    Number ScreenWidth() {
        return screen->Width();
    }
    Number ScreenHeight() {
        return screen->Height();
    }
    void DrawPixel(Number x, Number y, Number r, Number g, Number b) {
        screen->DirectDrawPixel(x, y, r, g, b);
    }
    void DrawString(Number base_x, Number base_y, String str) {
        Point pos(base_x, base_y);
        Color black(0, 0, 0, 0xFF);
        Color white(0xFF, 0xFF, 0xFF, 0xFF);
        screen_raw_canvas->FillText(pos, black, white, str);
    }
};

void Canvas::FillText(Point pos, Color fg, Color bg, String text) {
    Number w = BASIC_FONT_WIDTH,
           h = BASIC_FONT_HEIGHT;
    Number base_x = pos.X,
           base_y = pos.Y;
    Number i = 0,
           j = 0;
    for (auto it = text.Iterate(); it->HasCurrent(); it->Proceed()) {
        Char ch = it->Current();
        if (ch == '\n') {
            i = 0;
            j += 1;
            continue;
        }
        for (Number dy = 0; dy < h; dy += 1) {
            const Number* data = BasicFont::GetCharData(ch);
            for (Number dx = 0; dx < w; dx += 1) {
                Number x = base_x + (i * w) + dx;
                Number y = base_y + (j * h) + dy;
                Number offset = (w - dx - 1);
                bool is_fg = false;
                if (data != nullptr) {
                    is_fg = (data[dy] & (1 << offset));
                }
                if (is_fg) {
                    DrawPixel(x, y, fg.R, fg.G, fg.B, fg.A);
                } else {
                    DrawPixel(x, y, bg.R, bg.G, bg.B, bg.A);
                }
            }
        }
        i++;
    }
}

