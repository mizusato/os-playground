#include "scheduler.hpp"

namespace Scheduler {
    TaskScheduler* instance = nullptr;
    void Init() {
        if (instance != nullptr) {
            panic("scheduler: already initialized");
        }
        instance = new TaskScheduler;
    }
    TaskScheduler* GetInstance() {
        return instance;
    }
    void Cycle(TaskScheduler::CycleInfo* info) {
        instance->Cycle(info);
    }
    bool DispatchTimerEvent(TimerEvent ev) {
        return instance->DispatchEvent(ev);
    }
};

