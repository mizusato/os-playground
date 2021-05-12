#include "fonts.hpp"
#include "windows.hpp"


BackgroundWindow::BackgroundWindow(Color color, Point size): Window(Point(0,0), size), color(color) {
    flags.background = true;
    WindowManager::Add(this);
}

BackgroundWindow::~BackgroundWindow() {
    WindowManager::Remove(this);
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


BaseWindow::Options::Options():
    closable(true),
    borderSize(3),
    titleHeight(34),
    borderColor(Color(0x7F, 0x7F, 0x7F, 0x12)),
    titleColorInactive(Color(0x7F, 0x7F, 0x7F, 0xF3)),
    titleColorActive(Color(0xF3, 0x33, 0xA3, 0xF3)),
    titleFontColor(Color(0xFF, 0xFF, 0xFF, 0xFF)),
    contentColor(Color(0xEF, 0xEF, 0xEF, 0xF3)) {}

BaseWindow::State::State():
    title(""),
    dragging(false) {}

BaseWindow::BaseWindow(Point pos, Point size, String title, Options opts):
    Window(pos, (size + Point((opts.borderSize * 2), (opts.titleHeight + (opts.borderSize * 2))))),
    opts(Unique<Options>(new Options(opts))), state(Unique<State>(new State)) {
    state->title = title;
};

void BaseWindow::GetContentArea(Point* start, Point* span) {
    Number b = opts->borderSize;
    Number t = opts->titleHeight;
    start->X = b;
    start->Y = (t + b);
    span->X = (size.X - (2 * b));
    span->Y = (size.Y - t - (2 * b));
}

void BaseWindow::Render(Canvas& target, bool active) {
    Number b = opts->borderSize;
    Number t = opts->titleHeight;
    Color t_color = (active)? opts->titleColorActive: opts->titleColorInactive;
    target.FillRect(Point(b, (b + t)), (size - Point((2 * b), ((2 * b) + t))), opts->contentColor);
    target.FillRect(Point(0, 0), Point(size.X, b), opts->borderColor);
    target.FillRect(Point(0, (size.Y - b)), Point(size.X, b), opts->borderColor);
    target.FillRect(Point(0, b), Point(b, (size.Y - (2 * b))), opts->borderColor);
    target.FillRect(Point((size.X - b), b), Point(b, (size.Y - (2 * b))), opts->borderColor);
    target.FillRect(Point(b, b), Point((size.X - (2 * b)), t), t_color);
    Font* font = GetPrimaryFont();
    Color font_color = opts->titleFontColor;
    Number title_padding = ((t - font->Height()) / 2);
    Number p = (b + title_padding);
    target.FillText(Point((p + b), p), font_color, *font, state->title);
    if (opts->closable) {
        target.FillText(Point((size.X - (p + b) - font->Width()), p), font_color, *font, "X");
    }
    Point content_start, content_span;
    GetContentArea(&content_start, &content_span);
    auto content_viewport = target.SliceView(content_start, content_span);
    RenderContent(*content_viewport, active);
}

void BaseWindow::DispatchEvent(KeyboardEvent ev) {
    DispatchContentEvent(ev);
}

void BaseWindow::DispatchEvent(MouseEvent ev) {
    if (state->dragging) {
        if (ev.up || ev.out) {
            state->dragging = false;
        } else {
            auto ref_x = static_cast<SignedNumber>(state->dragPoint.X);
            auto ref_y = static_cast<SignedNumber>(state->dragPoint.Y);
            auto x = static_cast<SignedNumber>(ev.pos.X);
            auto y = static_cast<SignedNumber>(ev.pos.Y);
            auto dx = (x - ref_x);
            auto dy = (y - ref_y);
            auto new_x = (static_cast<SignedNumber>(position.X) + dx);
            auto new_y = (static_cast<SignedNumber>(position.Y) + dy);
            if (new_x < 0) { new_x = 0; }
            if (new_y < 0) { new_y = 0; }
            position = Point(Number(new_x), Number(new_y));
        }
    } else {
        Number b = opts->borderSize;
        Number t = opts->titleHeight;
        if (ev.down && (ev.alt || ev.pos.Y < (b + t))) {
            state->dragging = true;
            state->dragPoint = ev.pos;
        } else {
            Point start, span;
            GetContentArea(&start, &span);
            ev.pos = (ev.pos - start);
            DispatchContentEvent(ev);
        }
    }
}

