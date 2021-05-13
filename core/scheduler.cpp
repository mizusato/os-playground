#include "scheduler.hpp"

namespace Scheduler {
    TaskScheduler* instance = nullptr;
    void Init() {
        if (instance != nullptr) {
            panic("Scheduler: already initialized");
        }
        instance = new TaskScheduler;
    }
    TaskScheduler* GetInstance() {
        return instance;
    }
    bool Cycle() {
        return instance->Cycle();
    }
    void DispatchTimerEvent(TimerEvent ev) {
        instance->DispatchEvent(ev);
    }
};

