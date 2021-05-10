#ifndef SHELL_RUNTIME_HPP
#define SHELL_RUNTIME_HPP

#include "../core/shared.hpp"
#include "../core/list.hpp"
#include "../core/events.hpp"


class Variant {
protected:
    Variant() {};
    virtual ~Variant() {}
};

class AbstractConsole {
protected:
    AbstractConsole() {};
    virtual ~AbstractConsole() {};
public:
    // TODO: console APIs
};

class AbstractWindow {
protected:
    AbstractWindow() {};
    virtual ~AbstractWindow() {};
public:
    // TODO: window APIS
};

struct TaskContext {
    AbstractConsole* console;
    AbstractWindow*  window;
};

class Continuation {
protected:
    Continuation() {};
    virtual ~Continuation() {};
public:
    struct Result {
        bool hasNext;
        Shared<Continuation> next;
        Result(bool hasNext, Shared<Continuation> next): hasNext(hasNext), next(std::move(next)) {};
    };
    Result Run();
};

class Callback {
protected:
    Callback() {};
    virtual ~Callback() {};
public:
    Shared<Continuation> Call(Variant argument) const;
};

class Task final {
private:
    Number id;
    Shared<TaskContext> ctx;
    Unique<List<Shared<Continuation>>> runQueue;
    // TODO: List of EventListener (for RAII detaching)
};

class Program {
protected:
    Program() {};
    virtual ~Program() {};
public:
    String Name() const;
    virtual Shared<Continuation> Run() const;
};

class Scheduler final {
private:
    Unique<List<Shared<Task>>> runningTasks;
public:
    Number Start(Shared<Program> p, Shared<TaskContext> ctx);
    bool Kill(Number id);
    bool Cycle();
    void DispatchEvent(TimerEvent ev);
    void DispatchEvent(KeyboardEvent ev, AbstractWindow* window);
    void DispatchEvent(MouseEvent ev, AbstractWindow* window);
};

#endif

