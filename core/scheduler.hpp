#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "../shell/runtime.hpp"

namespace Scheduler {
    void Init();
    TaskScheduler* GetInstance();
    void Cycle(TaskScheduler::CycleInfo* info);
    bool DispatchTimerEvent(TimerEvent ev);
};

#endif

