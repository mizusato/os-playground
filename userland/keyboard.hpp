#ifndef USERLAND_KEYMON_HPP
#define USERLAND_KEYMON_HPP

#include "../shell/runtime.hpp"

namespace Userland {
    class KeyboardInspector final: public Program {
    private:
        class ShowEvent final: public Continuation {
        private:
            KeyboardEvent ev;
        public:
            ShowEvent(KeyboardEvent ev): ev(ev) {}
            Result Run(TaskContext& ctx, TaskStatus& status) override {
                String::Builder buf;
                buf.Write(String::Chr(ev.key));
                buf.Write(" ");
                buf.Write(String::Hex(ev.key));
                if (ev.shift) { buf.Write(" + shift"); }
                buf.Write("\n");
                if (ev.ctrl) { buf.Write("[ctrl] "); }
                if (ev.alt) { buf.Write("[alt] "); }
                ctx.window->SetContent(buf.Collect());
                return Result(nullptr, nullptr);
            }
        };
        class EventHandler final: public KeyboardEventHandler {
        public:
            Continuation* HandleEvent(KeyboardEvent ev) override {
                return new ShowEvent(ev);
            }
        };
        class Main final: public Continuation {
        public:
            Result Run(TaskContext& ctx, TaskStatus& status) override {
                return Result(nullptr, new EventHandler);
            }
        };
    public:
        String Name() const override { return "Keyboard Demo"; }
        Continuation* Run() const override {
            return new Main();
        }
    };
}

#endif

