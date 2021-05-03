#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include "widget.hpp"


struct TextStyle {
    Color color;
    TextStyle(Color color): color(color) {}
};

class TextDisplay final: public Widget {
private:
    struct Line {
        String content;
        TextStyle style;
        Line(String content, TextStyle style): content(content), style(style) {}
    };
    struct State {
        Number num_of_rows;
        Number num_of_columns;
        Number scroll_skip_rows;
        Number scroll_skip_columns;
        Unique<List<Line>> lines;
        State();
    };
    Unique<State> state;
public:
    TextDisplay(const Widget** focus);
    ~TextDisplay() {};
    void Render(Canvas& target, bool window_active) override;
    void ConsumeEvent(KeyboardEvent _) override {};
    void ConsumeEvent(MouseEvent ev) override;
    void Add(String content, TextStyle style);
    void Clear();
    enum ScrollDirection { Up, Down, Left, Right };
    void Scroll(ScrollDirection d);
};

#endif
