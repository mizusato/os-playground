#include "events.hpp"

namespace Events {
    FIFO<KeyboardEvent>* Keyboard;
    FIFO<MouseEvent>* Mouse;
    FIFO<TimerEvent>* TimerSecond;
    void Init() {
        Keyboard = new FIFO<KeyboardEvent>();
        Mouse = new FIFO<MouseEvent>();
        TimerSecond = new FIFO<TimerEvent>();
    }
}

