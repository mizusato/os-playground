#ifndef USERLAND_TIMER_HPP
#define USERLAND_TIMER_HPP

#include "../shell/runtime.hpp"

namespace Userland {
    class Ticker final: public Program {
    private:
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
        class EventHandler final: public TimerEventHandler {
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
                return Result(new ShowCount(0), new EventHandler());
            }
        };
    public:
        String Name() const override { return "Ticker"; }
        Continuation* Run() const override {
            return new Main();
        }
    };
}

#endif

