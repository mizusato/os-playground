#ifndef SHELL_RUNTIME_HPP
#define SHELL_RUNTIME_HPP

#include "../core/shared.hpp"
#include "../core/list.hpp"
#include "../core/events.hpp"


class AbstractConsole {
protected:
    AbstractConsole() {};
public:
    virtual ~AbstractConsole() {};
    virtual void TaskNotifyStarted(Number cmd, Number id) = 0;
    virtual void TaskNotifyKilled(Number cmd) = 0;
    virtual void TaskNotifyDone(Number cmd, bool ok, String msg) = 0;
    virtual void TaskPrintLog(Number cmd, String msg) = 0;
};

class AbstractWindow {
protected:
    AbstractWindow() {};
public:
    virtual ~AbstractWindow() {};
    virtual void Destroy() = 0;
    virtual void SetTitle(String title) = 0;
    virtual void SetContent(String content) = 0;
};

class EventHandler {
protected:
    EventHandler() {};
public:
    virtual ~EventHandler() {};
    enum Kind {
        Timer, Keyboard, Mouse
    };
    virtual Kind GetKind() = 0;
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
        Continuation* next;
        EventHandler* newHandler;
        Result(Continuation* next, EventHandler* newHandler):
            next(next), newHandler(newHandler) {};
    };
    virtual Result Run(TaskContext& ctx, TaskStatus& status) = 0;
};

class Program {
protected:
    Program() {};
public:
    virtual ~Program() {};
    virtual String Name() const = 0;
    virtual Continuation* Run() const = 0;
};

class TimerEventHandler: public EventHandler {
protected:
    TimerEventHandler() {};
public:
    virtual ~TimerEventHandler() {};
    virtual Continuation* HandleEvent(TimerEvent ev) = 0;
    Kind GetKind() override { return Timer; }
};

class KeyboardEventHandler: public EventHandler {
protected:
    KeyboardEventHandler() {};
public:
    virtual ~KeyboardEventHandler() {};
    virtual Continuation* HandleEvent(KeyboardEvent ev) = 0;
    Kind GetKind() override { return Keyboard; }
};

class MouseEventHandler: public EventHandler {
protected:
    MouseEventHandler() {};
public:
    virtual ~MouseEventHandler() {};
    virtual Continuation* HandleEvent(MouseEvent ev) = 0;
    Kind GetKind() override { return Mouse; }
};

class Task final {
public:
    Task(Number id, Unique<TaskContext> ctx, Continuation* k);
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
    void QueueContinuation(Continuation* k);
    void AttachEventHandler(EventHandler* handler);
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
    bool Kill(AbstractWindow *window);
    bool Cycle();
    void DispatchEvent(TimerEvent ev);
    void DispatchEvent(KeyboardEvent ev, AbstractWindow* window);
    void DispatchEvent(MouseEvent ev, AbstractWindow* window);
};

#endif

