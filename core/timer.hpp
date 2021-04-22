#ifndef TIMER_HPP
#define TIMER_HPP

namespace Timer {
    void Init();
};

extern "C" {
    void handleTimerInterrupt();
}

#endif

