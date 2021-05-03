#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "../core/graphics.hpp"
#include "../core/events.hpp"

class Widget {
public:
    Point position;
    Point size;
protected:
    const Widget** focus;
    bool focusable;
    Widget(const Widget** focus, bool focusable);
public:
    virtual ~Widget() {};
    virtual void Render(Canvas& target, bool window_active) = 0;
    virtual void ConsumeEvent(MouseEvent ev) = 0;
    virtual void ConsumeEvent(KeyboardEvent ev) = 0;
    virtual bool DispatchFocus(const Widget** previous, bool reversed);
    virtual void DispatchEvent(MouseEvent ev);
    virtual void DispatchEvent(KeyboardEvent ev);
};

class LinearLayout final: public Widget {
public:
    struct Item {
        Widget* widget;
        Number factor;
        Number computed;
    };
    enum Kind {
        Vertical,
        Horizontal
    };
private:
    Kind kind;
    Unique<List<Item>> items;
    Number num_of_items;
    MouseEvent prev_ev;
public:
    LinearLayout(const Widget** focus, Kind kind);
    ~LinearLayout() {};
    void Append(Widget* widget, Number factor);
    void Render(Canvas& target, bool window_active) override;
    void ConsumeEvent(MouseEvent _) override {};
    void ConsumeEvent(KeyboardEvent _) override {};
    bool DispatchFocus(const Widget** previous, bool reversed) override;
    void DispatchEvent(KeyboardEvent ev) override;
    void DispatchEvent(MouseEvent ev) override;
};

#endif

