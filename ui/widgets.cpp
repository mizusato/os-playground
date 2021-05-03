#include "fonts.hpp"
#include "widgets.hpp"


TextDisplay::State::State():
    num_of_rows(0),
    num_of_columns(0),
    scroll_skip_rows(0),
    scroll_skip_columns(0),
    lines(Unique<List<Line>>(new List<Line>())) {}

TextDisplay::TextDisplay(const Widget** focus):
    Widget(focus, false), state(Unique<State>(new State)) {}

void TextDisplay::Render(Canvas& target, bool _) {
    Number spacing = 1;
    Number pad_x = 6;
    Number pad_y = 3;
    if (target.Width() < (2 * pad_x)) { return; }
    if (target.Height() < (2 * pad_y)) { return; }
    Font* font = GetPrimaryFont();
    Number w = (target.Width() - (2 * pad_x)), h = (target.Height() - (2 * pad_y));
    Number bw = font->Width(), bh = (font->Height() + spacing);
    Number vw = (w / bw), vh = (h / bh);
    auto viewport_lines = Unique<List<Line>>(new List<Line>);
    Number count = 0;
    for (auto it = state->lines->Iterate(); it->HasCurrent(); it->Proceed()) {
        if (count >= vh) { break; };
        Line line = it->Current();
        viewport_lines->Prepend(line);
        count += 1;
    }
    Number i = 0;
    for (auto I = viewport_lines->Iterate(); I->HasCurrent(); I->Proceed()) {
        if (i < state->scroll_skip_rows) { continue; }
        Number y = (i - state->scroll_skip_rows);
        if (y >= vh) { continue; }
        Line line = I->Current();
        Color color = line.style.color;
        Number j = 0;
        auto buf = Unique<List<Char>>(new List<Char>());
        for (auto J = line.content.Iterate(); J->HasCurrent(); J->Proceed()) {
            if (j >= state->num_of_columns) { break; }
            if (j < state->scroll_skip_columns) { continue; }
            Number x = (j - state->scroll_skip_columns);
            if (x >= vw) { continue; }
            Char ch = J->Current();
            buf->Append(ch);
            j += 1;
        }
        Number top = (pad_y + (y * bh));
        target.FillText(Point(pad_x, top), color, *font, String(std::move(buf)));
        i += 1;
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
    }
}

void TextDisplay::Clear() {
    state = Unique<State>(new State);
}

void TextDisplay::Scroll(TextDisplay::ScrollDirection d) {

}

