#ifndef SHELL_RUNTIME_HPP
#define SHELL_RUNTIME_HPP

#include "../core/shared.hpp"
#include "../core/list.hpp"
#include "../core/events.hpp"


class AbstractConsole {
protected:
    AbstractConsole() {};
    virtual ~AbstractConsole() {};
public:
    virtual void TaskNotifyStarted(Number cmd) = 0;
    virtual void TaskNotifyKilled(Number cmd) = 0;
    virtual void TaskNotifyDone(Number cmd, bool ok, String msg) = 0;
    virtual void TaskPrintLog(Number cmd, String msg) = 0;
};

class AbstractWindow {
protected:
    AbstractWindow() {};
    virtual ~AbstractWindow() {};
public:
    virtual void Destroy() = 0;
    virtual void SetTitle(String title) = 0;
    virtual void SetContent(String content) = 0;
};

struct TaskStatus {
    bool killed;
    bool exited;
    bool successful;
    String resultMessage;
    TaskStatus();
};

struct TaskContext {
    AbstractConsole* console;
    AbstractWindow*  window;
    Number consoleCommandNumber;
};

class Continuation {
protected:
    Continuation() {};
public:
    virtual ~Continuation() {};
    struct Result {
        bool hasNext;
        Shared<Continuation> next;
        Result(bool hasNext, Shared<Continuation> next): hasNext(hasNext), next(std::move(next)) {};
    };
    virtual Result Run(TaskContext& ctx, TaskStatus& status) = 0;
};

class Program {
protected:
    Program() {};
    virtual ~Program() {};
public:
    virtual String Name() const = 0;
    virtual Shared<Continuation> Run() const = 0;
};

class TimerEventHandler {
protected:
    TimerEventHandler() {};
public:
    virtual ~TimerEventHandler() {};
    virtual Shared<Continuation> HandleEvent(TimerEvent ev) = 0;
};

class KeyboardEventHandler {
protected:
    KeyboardEventHandler() {};
public:
    virtual ~KeyboardEventHandler() {};
    virtual Shared<Continuation> HandleEvent(KeyboardEvent ev) = 0;
};

class MouseEventHandler {
protected:
    MouseEventHandler() {};
public:
    virtual ~MouseEventHandler() {};
    virtual Shared<Continuation> HandleEvent(MouseEvent ev) = 0;
};

class Task final {
public:
    Task(Number id, Unique<TaskContext> ctx, Shared<Continuation> k);
    ~Task() {};
    Number id;
    Unique<TaskContext> ctx;
    Unique<TaskStatus> status;
    Unique<List<Shared<Continuation>>> runQueue;
    bool NothingToDo() const;
    void ProcessEvent(TimerEvent ev);
    void ProcessEvent(KeyboardEvent ev);
    void ProcessEvent(MouseEvent ev);
    struct EventHandlers {
        Unique<List<Shared<TimerEventHandler>>> timerHandlers;
        Unique<List<Shared<KeyboardEventHandler>>> keyboardHandlers;
        Unique<List<Shared<MouseEventHandler>>> mouseHandlers;
        EventHandlers();
        bool AllEmpty() const;
    };
    Unique<EventHandlers> handlers;
};

class TaskScheduler final {
private:
    Unique<List<Shared<Task>>> runningTasks;
    Number nextTaskId;
public:
    TaskScheduler();
    ~TaskScheduler() {};
    Number Start(Shared<Program> p, Unique<TaskContext> ctx);
    bool Kill(Number id);
    bool Cycle();
    void DispatchEvent(TimerEvent ev);
    void DispatchEvent(KeyboardEvent ev, AbstractWindow* window);
    void DispatchEvent(MouseEvent ev, AbstractWindow* window);
};

#endif

