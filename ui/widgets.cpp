#include "fonts.hpp"
#include "widgets.hpp"


TextDisplay::State::State():
    num_of_rows(0),
    num_of_columns(0),
    scroll_skip_rows(0),
    scroll_skip_columns(0),
    current_viewport_rows(0),
    current_viewport_columns(0),
    lines(Unique<List<Line>>(new List<Line>())) {}

TextDisplay::TextDisplay(const Widget** focus):
    Widget(focus, false), state(Unique<State>(new State)) {}

void TextDisplay::Render(Canvas& target, bool _) {
    Number scrollbar_size = 10;
    if (target.Width() <= scrollbar_size) { return; }
    if (target.Height() <= scrollbar_size) { return; }
    Number content_width = (target.Width() - scrollbar_size);
    Number content_height = (target.Height() - scrollbar_size);
    Number spacing = 1;
    Number pad_x = 6;
    Number pad_y = 3;
    if (content_width <= (2 * pad_x)) { return; }
    if (content_height <= (2 * pad_y)) { return; }
    Font* font = GetPrimaryFont();
    Number w = (content_width - (2 * pad_x)), h = (content_height - (2 * pad_y));
    Number bw = font->Width(), bh = (font->Height() + spacing);
    Number vw = (w / bw), vh = (h / bh);
    state->current_viewport_columns = vw, state->current_viewport_rows = vh;
    auto viewport_lines = Unique<List<Line>>(new List<Line>());
    Number depth = 0;
    for (auto I = state->lines->Iterate(); I->HasCurrent(); I->Proceed(), depth++) {
        if (depth < state->scroll_skip_rows) { continue; }
        Number count = (depth - state->scroll_skip_rows);
        if (count >= vh) { break; }
        viewport_lines->Prepend(I->Current());
    }
    Number i = 0;
    for (auto I = viewport_lines->Iterate(); I->HasCurrent(); I->Proceed(), i++) {
        Number y = i;
        Line line = I->Current();
        Color color = line.style.color;
        auto buf = Unique<List<Char>>(new List<Char>());
        Number j = 0;
        for (auto J = line.content.Iterate(); J->HasCurrent(); J->Proceed(), j++) {
            if (j < state->scroll_skip_columns) { continue; }
            Number count = (j - state->scroll_skip_columns);
            if (count >= vw) { break; }
            Char ch = J->Current();
            buf->Append(ch);
        }
        Number top = (pad_y + (y * bh));
        target.FillText(Point(pad_x, top), color, *font, String(std::move(buf)));
    }
    Color scrollbar_color(0xA3, 0xA3, 0xA3, 0x7F);
    if (state->num_of_rows > vh) {
        Number total = state->num_of_rows;
        Number start = state->scroll_skip_rows;
        Number span = vh;
        Number end_y = (target.Height() - ((target.Height() * start) / total));
        Number span_y = ((target.Height() * span) / total);
        Number start_y = (end_y >= span_y)? (end_y - span_y): 0;
        Point p_start(content_width, start_y);
        Point p_span(scrollbar_size, span_y);
        target.FillRect(p_start, p_span, scrollbar_color);
    }
    if (state->num_of_columns > vw) {
        Number total = state->num_of_columns;
        Number start = state->scroll_skip_columns;
        Number span = ((start + vw) <= total)? vw: (total - start);
        Number start_x = ((content_width * start) / total);
        Number span_x = ((content_width * span) / total);
        Point p_start(start_x, content_height);
        Point p_span(span_x, scrollbar_size);
        target.FillRect(p_start, p_span, scrollbar_color);
    }
}

void TextDisplay::ConsumeEvent(MouseEvent ev) {

}

void TextDisplay::Add(String content, TextStyle style) {
    auto buf = Unique<List<Char>>(new List<Char>());
    bool buf_empty = true;
    Number added_rows = 0;
    Number max_cols = 0;
    Number cols = 0;
    for (auto it = content.Iterate(); it->HasCurrent(); it->Proceed()) {
        Char ch = it->Current();
        if (ch == '\n') {
            added_rows += 1;
            state->lines->Prepend(Line(String(std::move(buf)), style));
            if (cols > max_cols) { max_cols = cols; }
            cols = 0;
            buf = Unique<List<Char>>(new List<Char>());
            buf_empty = true;
        } else {
            buf->Append(ch);
            buf_empty = false;
            cols += 1;
        }
    }
    if ( !(buf_empty) ) {
        added_rows += 1;
        state->lines->Prepend(Line(String(std::move(buf)), style));
        if (cols > max_cols) { max_cols = cols; }
    }
    state->num_of_rows += added_rows;
    if (max_cols > state->num_of_columns) {
        state->num_of_columns = max_cols;
    }
    if (state->scroll_skip_rows != 0) {
        state->scroll_skip_rows += added_rows;
        state->scroll_skip_columns = 0;
    }
}

void TextDisplay::Clear() {
    state = Unique<State>(new State);
}

void TextDisplay::Scroll(TextDisplay::ScrollDirection d) {
    Number r = state->num_of_rows;
    Number rv = state->current_viewport_rows;
    Number rs_max = (r >= rv)? (r - rv): 0;
    Number& rs = state->scroll_skip_rows;
    Number c = state->num_of_columns;
    Number cv = state->current_viewport_columns;
    Number cs_max = (c >= cv)? (c - cv): 0;
    Number& cs = state->scroll_skip_columns;
    if (d == Up) {
        if ((rs + 1) <= rs_max) { rs += 1; }
    } else if (d == Down) {
        if (rs > 0) { rs -= 1; }
    } else if (d == Right) {
        if ((cs + 1) <= cs_max) { cs += 1; }
    } else if (d == Left) {
        if (cs > 0) { cs -= 1; }
    }
}

