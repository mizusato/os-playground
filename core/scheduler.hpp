#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "../shell/runtime.hpp"

namespace Scheduler {
    void Init();
    TaskScheduler* GetInstance();
    bool Cycle();
    bool DispatchTimerEvent(TimerEvent ev);
};

#endif

