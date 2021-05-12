#include "runtime.hpp"


TaskStatus::TaskStatus():
    exited(false),
    successful(true),
    resultMessage("exited") {}

Task::EventHandlers::EventHandlers():
    timerHandlers(Unique<List<Shared<TimerEventHandler>>>(new List<Shared<TimerEventHandler>>())),
    keyboardHandlers(Unique<List<Shared<KeyboardEventHandler>>>(new List<Shared<KeyboardEventHandler>>())),
    mouseHandlers(Unique<List<Shared<MouseEventHandler>>>(new List<Shared<MouseEventHandler>>())) {}

bool Task::EventHandlers::AllEmpty() const {
    return (timerHandlers->Empty() &&
            keyboardHandlers->Empty() &&
            mouseHandlers->Empty());
}

Task::Task(Number id, Unique<TaskContext> ctx, Shared<Continuation> k):
    id(id),
    ctx(std::move(ctx)),
    status(Unique<TaskStatus>(new TaskStatus)),
    runQueue(Unique<List<Shared<Continuation>>>(new List<Shared<Continuation>>(k))),
    handlers(Unique<EventHandlers>(new EventHandlers)) {};

bool Task::NothingToDo() const {
    return (runQueue->Empty() && handlers->AllEmpty());
}

void Task::ProcessEvent(TimerEvent ev) {
    for (auto it = handlers->timerHandlers->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto h = it->Current();
        runQueue->Append(h->HandleEvent(ev));
    }
}

void Task::ProcessEvent(KeyboardEvent ev) {
    for (auto it = handlers->keyboardHandlers->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto h = it->Current();
        runQueue->Append(h->HandleEvent(ev));
    }
}

void Task::ProcessEvent(MouseEvent ev) {
    for (auto it = handlers->mouseHandlers->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto h = it->Current();
        runQueue->Append(h->HandleEvent(ev));
    }
}


TaskScheduler::TaskScheduler():
    runningTasks(Unique<List<Shared<Task>>>(new List<Shared<Task>>)),
    nextTaskId(7) {}

Number TaskScheduler::Start(Shared<Program> p, Unique<TaskContext> ctx) {
    Number id = nextTaskId;
    nextTaskId += 1;
    auto task = Shared<Task>(new Task(id, std::move(ctx), p->Run()));
    runningTasks->Append(task);
    {
        String::Builder buf;
        buf.Write("[#"); buf.Write(String(id)); buf.Write("] ");
        buf.Write(p->Name());
        String title = buf.Collect();
        ctx->window->SetTitle(title);
        ctx->console->TaskNotifyStarted(ctx->consoleCommandNumber);
    }
    return id;
}

bool TaskScheduler::Kill(Number id) {
    bool found = false;
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->id == id) {
            found = true;
            task->status->killed = true;
            break;
        }
    }
    return found;
}

bool TaskScheduler::Cycle() {
    auto remaining = Unique<List<Shared<Task>>>(new List<Shared<Task>>());
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->status->killed) {
            task->ctx->window->Destroy();
            task->ctx->console->TaskNotifyKilled(task->ctx->consoleCommandNumber);
        } else if (task->status->exited || task->NothingToDo()) {
            task->ctx->window->Destroy();
            Number cmd = task->ctx->consoleCommandNumber;
            bool ok = task->status->successful;
            String msg = task->status->resultMessage;
            task->ctx->console->TaskNotifyDone(cmd, ok, msg);
        } else {
            remaining->Append(task);
        }
    }
    runningTasks = std::move(remaining);
    bool somethingExecuted = false;
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->runQueue->NotEmpty()) {
            auto k = task->runQueue->Shift();
            auto result = k->Run(*(task->ctx), *(task->status));
            if (result.hasNext) {
                task->runQueue->Append(result.next);
            }
            somethingExecuted = true;
        }
    }
    return somethingExecuted;
}

void TaskScheduler::DispatchEvent(TimerEvent ev) {
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        task->ProcessEvent(ev);
    }
}

void TaskScheduler::DispatchEvent(KeyboardEvent ev, AbstractWindow* window) {
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->ctx->window == window) {
            task->ProcessEvent(ev);
        }
    }
}

void TaskScheduler::DispatchEvent(MouseEvent ev, AbstractWindow* window) {
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->ctx->window == window) {
            task->ProcessEvent(ev);
        }
    }
}

