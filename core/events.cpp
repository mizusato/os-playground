#include "events.hpp"

namespace Events {
    FIFO<KeyboardEvent>* Keyboard;
    FIFO<MouseEvent>* Mouse;
    FIFO<TimerEvent>* Timer;
    void Init() {
        Keyboard = new FIFO<KeyboardEvent>();
        Mouse = new FIFO<MouseEvent>();
        Timer = new FIFO<TimerEvent>();
    }
}

