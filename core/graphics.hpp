#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../boot.h"
#include "types.h"
#include "string.hpp"
#include "font.hpp"


class Point {
public:
    Number X;
    Number Y;
    Point(): X(0), Y(0) {};
    Point(Number x, Number y): X(x), Y(y) {};
};
Point operator + (Point a, Point b);
Point operator - (Point a, Point b);

class Color {
public:
    Byte R;
    Byte G;
    Byte B;
    Byte A;
    Color(Byte r, Byte g, Byte b, Byte a): R(r), G(g), B(b), A(a) {};
};

class Canvas {
protected:
    Canvas() {};
public:
    virtual ~Canvas() {};
    virtual Number Width() const = 0;
    virtual Number Height() const = 0;
    virtual void DrawPixel(Number x, Number y, Number r, Number g, Number b, Number a) = 0;
    Unique<Canvas> SliceView(Point pos, Point size);
    void FillText(Point pos, Color color, const Font& font, String text);
};

namespace Graphics {
    void Init(GraphicsInfo* gfxInfo);
    Number ScreenWidth();
    Number ScreenHeight();
    Font* GetBasicFont();
    Canvas* GetScreenCanvas();
    void FlushScreenCanvas();
    void DrawPixel(Number x, Number y, Number r, Number g, Number b);
    void DrawString(Number base_x, Number base_y, String str);
};

#endif

