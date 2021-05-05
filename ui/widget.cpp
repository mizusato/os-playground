#include "widget.hpp"


Widget::Widget(const Widget** focus, bool focusable):
    position(Point(0,0)), size(Point(100,100)), focus(focus), focusable(focusable) {};

bool Widget::DispatchFocus(const Widget** previous, bool _) {
    if (focusable) {
        if (*previous == *focus || *focus == nullptr) {
            *focus = this;
            return true;
        } else {
            *previous = this;
            return false;
        }
    } else {
        return false;
    }
}

void Widget::DispatchEvent(KeyboardEvent ev) {
    if (this == *focus) {
        ConsumeEvent(ev);
    }
}

void Widget::DispatchEvent(MouseEvent ev) {
    if (focusable && ev.down) {
        *focus = this;
    }
    ConsumeEvent(ev);
}


LinearLayout::LinearLayout(const Widget** focus, Kind kind):
    Widget(focus, false),
    kind(kind),
    items(Unique<List<Item>>(new List<Item>())),
    num_of_items(0) {}

void LinearLayout::Append(Widget* widget, Number factor) {
    Item item;
    item.widget = widget;
    item.factor = factor;
    items->Append(item);
    num_of_items += 1;
}

void LinearLayout::Render(Canvas& target, bool window_active) {
    Number w = target.Width();
    Number h = target.Height();
    position = Point(0, 0);
    size = Point(w, h);
    bool horizontal = (kind == Horizontal);
    Number total = horizontal? w: h;
    Number total_factor = 0;
    for (auto it = items->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto item = it->Current();
        auto given = item.widget->size;
        if (item.factor == 0) {
            Number fixed = (horizontal? given.X: given.Y);
            if (total < fixed) { panic("layout failure"); };
            total -= fixed; 
        }
        total_factor += item.factor;
    }
    Number total_computed = 0;
    for (auto it = items->Iterate(); it->HasCurrent(); it->Proceed()) {
        Item& item = it->Current();
        if (item.factor != 0) {
            Number computed = ((total * item.factor) / total_factor);
            item.computed = computed;
            total_computed += computed;
        }
    }
    Number remainder = (total - total_computed);
    for (auto it = items->Iterate(); it->HasCurrent(); it->Proceed()) {
        Item& item = it->Current();
        if (item.factor != 0) {
            item.computed += remainder;
            break;
        }
    }
    Number pos_x = 0;
    Number pos_y = 0;
    for (auto it = items->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto item = it->Current();
        auto widget = item.widget;
        widget->position = Point(pos_x, pos_y);
        if (item.factor != 0) {
            widget->size.X = horizontal? (item.computed): w;
            widget->size.Y = horizontal? h: (item.computed);
        } else {
            widget->size.X = horizontal? (widget->size.X): w;
            widget->size.Y = horizontal? h: (widget->size.Y);
        }
        if (horizontal) {
            pos_x += widget->size.X;
        } else {
            pos_y += widget->size.Y;
        }
        auto viewport = target.SliceView(widget->position, widget->size);
        widget->Render(*viewport, window_active);
    }
}

bool LinearLayout::DispatchFocus(const Widget** previous, bool reversed) {
    auto widgets = Unique<List<Widget*>>(new List<Widget*>);
    for (auto it = items->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto item = it->Current();
        auto widget = item.widget;
        if (reversed) {
            widgets->Prepend(widget);
        } else {
            widgets->Append(widget);
        }
    }
    for (auto it = widgets->Iterate(); it->HasCurrent(); it->Proceed()) {
        bool ok = it->Current()->DispatchFocus(previous, reversed);
        if (ok) {
            return true;
        }
    }
    return false;
}

void LinearLayout::DispatchEvent(KeyboardEvent ev) {
    for (auto it = items->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto item = it->Current();
        auto widget = item.widget;
        widget->DispatchEvent(ev);
    }
}

void LinearLayout::DispatchEvent(MouseEvent ev) {
    for (auto it = items->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto item = it->Current();
        auto widget = item.widget;
        bool contains_this_ev = InArea(ev.pos, position, size);
        bool contains_prev_ev = InArea(prev_ev.pos, position, size);
        if (contains_prev_ev && !(contains_this_ev)) {
            MouseEvent out_ev;
            out_ev.pos = prev_ev.pos;
            out_ev.out = true;
            widget->DispatchEvent(out_ev);
        }
        if (contains_this_ev) {
            if ( !(contains_prev_ev) ) {
                ev.in = true;
            }
            ev.pos = (ev.pos - widget->position);
            widget->DispatchEvent(ev);
            break;
        }
    }
    prev_ev = ev;
}


