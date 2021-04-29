#include "../boot.h"
#include "graphics.hpp"


Point operator + (Point a, Point b) { return Point((a.X + b.X), (a.Y + b.Y)); }
Point operator - (Point a, Point b) { return Point((a.X - b.X), (a.Y - b.Y)); }

Byte ApplyAlpha(Byte fg_, Byte bg_, Byte alpha_) {
    double fg = (double)(fg_);
    double bg = (double)(bg_);
    double alpha = ((double)(alpha_) / 255);
    Number raw = (Number)(bg + (fg - bg) * alpha);
    if (raw < 0) { raw = 0; }
    if (raw > 255) { raw = 255; }
    Byte clamped = (Byte)(raw);
    return clamped;
}

Byte ApplyLightness(Byte scalar_, Byte l_) {
    double scalar = (double)(scalar_);
    double l = ((double)(l_) / 255);
    return (Byte)(Number)(255 - ((255 - scalar) * (1 - l)));
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
        *r = (p & 0xFF); *g = ((p & 0xFF00) >> 8); *b = ((p & 0xFF0000) >> 16);
    }
};
class PFH_BGR final: public PixelFormatHandler {
    Dword MakePixel(Byte r, Byte g, Byte b) override {
        return (b | (g << 8) | (r << 16));
    }
    void ReadPixel(Dword p, Byte* r, Byte* g, Byte* b) override {
        *r = ((p & 0xFF0000) >> 16); *g = ((p & 0xFF00) >> 8); *b = (p & 0xFF);
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
    Dword* mirrorBuffer;
    bool*  mirrorRowDirty;
    bool   mirrorDirty;
    void initPixelFormatHandler() {
        PixelFormat pf = gfxInfo.pixelFormat;
        if (pf == PF_RGB) { pfh = new PFH_RGB; } else
        if (pf == PF_BGR) { pfh = new PFH_BGR; } else
                          { pfh = new PFH_Unsupported; }
    }
    void initBuffers() {
        Number numberOfPixels = (gfxInfo.screenHeight * gfxInfo.screenBufferWidth);
        void* area = Heap::RequestStatic(numberOfPixels * sizeof(Dword));
        draftBuffer = reinterpret_cast<Dword*>(area);
        area = Heap::RequestStatic(numberOfPixels * sizeof(Dword));
        mirrorBuffer = reinterpret_cast<Dword*>(area);
        for (Number i = 0; i < numberOfPixels; i += 1) {
            draftBuffer[i] = 0;
            mirrorBuffer[i] = 0;
        }
        area = Heap::RequestStatic(gfxInfo.screenHeight * sizeof(bool));
        mirrorRowDirty = reinterpret_cast<bool*>(area);
        for (Number i = 0; i < gfxInfo.screenHeight; i += 1) {
            mirrorRowDirty[i] = false;
        }
        mirrorDirty = false;
    }
public:
    Screen(GraphicsInfo* gfxInfo): gfxInfo(*gfxInfo) {
        initPixelFormatHandler();
        initBuffers();
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
        for (Number y = 0; y < gfxInfo.screenHeight; y += 1) {
            for (Number x = 0; x < gfxInfo.screenViewportWidth; x += 1) {
                Number offset = (x + (y * gfxInfo.screenBufferWidth));
                Dword* draftPixel = (draftBuffer + offset);
                Dword* mirrorPixel = (mirrorBuffer + offset);
                if (*draftPixel != *mirrorPixel) {
                    mirrorDirty = true;
                    mirrorRowDirty[y] = true;
                    *mirrorPixel = *draftPixel;
                }
            }
        }
        if (mirrorDirty) {
            mirrorDirty = false;
            for (Number y = 0; y < gfxInfo.screenHeight; y += 1) {
                if (mirrorRowDirty[y]) {
                    mirrorRowDirty[y] = false;
                    for (Number x = 0; x < gfxInfo.screenViewportWidth; x += 1) {
                        Number offset = (x + (y * gfxInfo.screenBufferWidth));
                        gfxInfo.framebuffer[offset] = mirrorBuffer[offset];
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
        if (x < Width() && y < Height()) {
            screen->DirectDrawPixel(x, y, r, g, b);
        }
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
        if (x < Width() && y < Height()) {
            screen->DraftSetPixel(x, y, r, g, b, a);
        }
    }
};

namespace Graphics {
    Screen* screen = nullptr;
    Canvas* screen_raw_canvas;
    Canvas* screen_draft_canvas;
    Font* basic_font;
    void Init(GraphicsInfo* gfxInfo) {
        if (screen != nullptr) {
            panic("Graphics::Init(): already initialized");
        }
        screen = new Screen(gfxInfo);
        screen_raw_canvas = new ScreenRawCanvas(screen);
        screen_draft_canvas = new ScreenDraftCanvas(screen);
        basic_font = new BasicFont;
        for (Number y = 0; y < screen->Height(); y += 1) {
            for (Number x = 0; x < screen->Width(); x += 1) {
                screen->DirectDrawPixel(x, y, 0, 0, 0);
            }
        }
    }
    Number ScreenWidth() {
        return screen->Width();
    }
    Number ScreenHeight() {
        return screen->Height();
    }
    Font* GetBasicFont() {
        return basic_font;
    }
    Canvas* GetScreenCanvas() {
        return screen_draft_canvas;
    }
    void FlushScreenCanvas() {
        screen->DraftCommit();
    }
    void DrawPixel(Number x, Number y, Number r, Number g, Number b) {
        screen->DirectDrawPixel(x, y, r, g, b);
    }
    void DrawString(Number base_x, Number base_y, String str) {
        for (Number dy = 0; dy < basic_font->Height(); dy += 1) {
            for (Number x = 0; x < screen->Width(); x += 1) {
                screen->DirectDrawPixel(x, (base_y + dy), 0xFF, 0xFF, 0xFF);
            }
        }
        Point pos(base_x, base_y);
        Color black(0, 0, 0, 0xFF);
        screen_raw_canvas->FillText(pos, black, *basic_font, str);
    }
};

class CanvasSlice final: public Canvas {
public:
    Canvas* base;
    Point pos;
    Point size;
    CanvasSlice(Canvas* base, Point pos, Point size): base(base), pos(pos), size(size) {};
    ~CanvasSlice() {};
    Number Width() const override {
        return size.X;
    }
    Number Height() const override {
        return size.Y;
    }
    void DrawPixel(Number x, Number y, Number r, Number g, Number b, Number a) override {
        if ((x < size.X) && (y < size.Y)) {
            x += pos.X;
            y += pos.Y;
            if ((x < base->Width()) && (y < base->Height())) {
                base->DrawPixel(x, y, r, g, b, a);
            }
        }
    }
};

Unique<Canvas> Canvas::SliceView(Point pos, Point size) {
    return Unique<Canvas>(new CanvasSlice(this, pos, size));
}

void Canvas::FillText(Point pos, Color color, const Font& font, String text) {
    Number w = font.Width(),
           h = font.Height();
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
            for (Number dx = 0; dx < w; dx += 1) {
                Number x = base_x + (i * w) + dx;
                Number y = base_y + (j * h) + dy;
                Byte p = font.GetPixel(ch, dx, dy);
                Byte data_alpha = (0xFF - p);
                Number a = (Number(color.A) * Number(data_alpha) / 255);
                DrawPixel(x, y, color.R, color.G, color.B, a);
            }
        }
        i++;
    }
}

