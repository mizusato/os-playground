#include "fonts.hpp"
#include "windows.hpp"

BackgroundWindow::BackgroundWindow(Color color, Point size): Window(Point(0,0), size), color(color) {
    flags.background = true;
}

void BackgroundWindow::Render(Canvas& target, bool _) {
    Number w = target.Width();
    Number h = target.Height();
    Number r = color.R;
    Number g = color.G;
    Number b = color.B;
    Number a = color.A;
    Number t1 = 0;
    Number t2 = 0;
    for (Number y = 0; y < h; y += 1) {
        if (t2 % 2 == 1) {
            if (t1 == 0 && r < 0xFF) { r += 1; }
            if (t1 == 1 && g < 0xFF) { g += 1; }
            if (t1 == 2 && b < 0xFF) { b += 1; }
            t1 = (t1 + 1) % 3;
        }
        t2 += 1;
        for (Number x = 0; x < w; x += 1) {
            target.DrawPixel(x, y, r, g, b, a);
        }
    }
    Font* font = GetPrimaryFont();
    const char* info = "Experimental OS";
    Number n = 0;
    for (auto it = info; *it != 0; it++) { n++; }
    Number info_x = (w - (font->Width() * n) - 20);
    Color info_color(0xFF, 0xFF, 0xFF, 0xFF);
    target.FillText(Point(info_x, 20), info_color, *font, info);
}

