#ifndef USERLAND_COUNTDOWN_HPP
#define USERLANG_COUNTDOWN_HPP

#include "../shell/runtime.hpp"

namespace Userland {
    class Countdown final: public Program {
    private:
        Number initial;
        class Next final: public Continuation {
        private:
            Number count;
        public:
            Next(Number count): count(count) {}
            Result Run(TaskContext& ctx, TaskStatus& status) override {
                ctx.window->SetContent(String(count));
                if (count > 0) {
                    return Result(new Next(count - 1), nullptr);
                } else {
                    return Result(nullptr, nullptr);
                }
            }
        };
    public:
        Countdown(Number initial): initial(initial) {};
        String Name() const override { return "Countdown"; }
        Continuation* Run() const override {
            return new Next(initial);
        }
    };
}

#endif

