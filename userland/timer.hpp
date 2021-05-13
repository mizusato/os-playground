#ifndef USERLAND_TIMER_HPP
#define USERLAND_TIMER_HPP

#include "../shell/runtime.hpp"

namespace Userland {
    class Timer final: public Program {
    public:
        class ShowCount final: public Continuation {
        private:
            Number count;
        public:
            ShowCount(Number count): count(count) {};
            Result Run(TaskContext& ctx, TaskStatus& status) override {
                ctx.window->SetContent(String(count));
                return Result(nullptr, nullptr);
            }
        };
        class TimerHandler final: public TimerEventHandler {
        private:
            Number current = 0;
        public:
            Continuation* HandleEvent(TimerEvent ev) override {
                if ((ev.count % 10) == 0) {
                    // 1000ms (1s) interval
                    Number count = current;
                    current += 1;
                    return new ShowCount(count);
                } else {
                    return nullptr;
                }
            }
        };
        class Main final: public Continuation {
        public:
            Result Run(TaskContext& ctx, TaskStatus& status) override {
                return Result(new ShowCount(0), new TimerHandler());
            }
        };
        String Name() const override { return "Timer"; }
        Continuation* Run() const override {
            return new Main();
        }
    };
}

#endif

