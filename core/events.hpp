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
    Number  button;  // TODO: bool: btnLeft, btnRight, down, up, in, out
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

