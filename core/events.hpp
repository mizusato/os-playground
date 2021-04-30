#ifndef EVENTS_HPP
#define EVENTS_HPP

#include "graphics.hpp"
#include "fifo.hpp"

struct KeyboardEvent {
    Char  key;
    bool  shift = false;
    bool  ctrl = false;
    bool  alt = false;
} __attribute__((packed));

struct MouseEvent {
    Point   pos;
    bool    btnLeft = false;
    bool    btnRight = false;
    bool    down = false;
    bool    up = false;
    bool    in = false;
    bool    out = false;
    bool    ctrl = false;
    bool    alt = false;
} __attribute__((packed));

struct TimerEvent {
    Number  count;
};

namespace Events {
    extern FIFO<KeyboardEvent>* Keyboard;
    extern FIFO<MouseEvent>* Mouse;
    extern FIFO<TimerEvent>* TimerSecond;
    void Init();
};

#endif

