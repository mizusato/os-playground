#ifndef USERLAND_MOUSE_HPP
#define USERLAND_MOUSE_HPP

#include "../shell/runtime.hpp"

namespace Userland {
    class MouseInspector final: public Program {
    private:
        class ShowEvent final: public Continuation {
        private:
            MouseEvent ev;
        public:
            ShowEvent(MouseEvent ev): ev(ev) {}
            Result Run(TaskContext& ctx, TaskStatus& status) override {
                String::Builder buf;
                buf.Write("mouse:\n");
                buf.Write("x: "); buf.Write(String(ev.pos.X)); buf.Write("\n");
                buf.Write("y: "); buf.Write(String(ev.pos.Y)); buf.Write("\n");
                buf.Write("left: "); buf.Write(String(ev.btnLeft)); buf.Write("\n");
                buf.Write("alt: "); buf.Write(String(ev.alt));
                ctx.window->SetContent(buf.Collect());
                return Result(nullptr, nullptr);
            }
        };
        class EventHandler final: public MouseEventHandler {
        public:
            Continuation* HandleEvent(MouseEvent ev) override {
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
        String Name() const override { return "Mouse Demo"; }
        Continuation* Run() const override {
            return new Main();
        }
    };
}

#endif

