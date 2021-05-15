#include "runtime.hpp"


TaskStatus::TaskStatus():
    killed(false),
    consoleDisposed(false),
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

Task::Task(Number id, Unique<TaskContext> ctx, Continuation* k):
    id(id),
    ctx(std::move(ctx)),
    status(Unique<TaskStatus>(new TaskStatus)),
    runQueue(Unique<List<Shared<Continuation>>>(new List<Shared<Continuation>>(Shared<Continuation>(k)))),
    handlers(Unique<EventHandlers>(new EventHandlers)) {};

bool Task::NothingToDo() const {
    return (runQueue->Empty() && handlers->AllEmpty());
}

bool Task::ProcessEvent(TimerEvent ev) {
    bool consumed = false;
    for (auto it = handlers->timerHandlers->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto h = it->Current();
        auto k = h->HandleEvent(ev);
        if (k != nullptr) {
            QueueContinuation(k);
            consumed = true;
        }
    }
    return consumed;
}

void Task::ProcessEvent(KeyboardEvent ev) {
    for (auto it = handlers->keyboardHandlers->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto h = it->Current();
        auto k = h->HandleEvent(ev);
        if (k != nullptr) {
            QueueContinuation(k);
        }
    }
}

void Task::ProcessEvent(MouseEvent ev) {
    for (auto it = handlers->mouseHandlers->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto h = it->Current();
        auto k = h->HandleEvent(ev);
        if (k != nullptr) {
            QueueContinuation(k);
        }
    }
}

void Task::QueueContinuation(Continuation* k) {
    if (k == nullptr) {
        panic("task: cannot add a null continuation");
    }
    runQueue->Append(Shared<Continuation>(k));
}

void Task::AttachEventHandler(EventHandler* handler) {
    auto kind = handler->GetKind();
    if (kind == EventHandler::Timer) {
        auto h = reinterpret_cast<TimerEventHandler*>(handler);
        handlers->timerHandlers->Append(Shared<TimerEventHandler>(h));
        return;
    }
    if (kind == EventHandler::Keyboard) {
        auto h = reinterpret_cast<KeyboardEventHandler*>(handler);
        handlers->keyboardHandlers->Append(Shared<KeyboardEventHandler>(h));
        return;
    }
    if (kind == EventHandler::Mouse) {
        auto h = reinterpret_cast<MouseEventHandler*>(handler);
        handlers->mouseHandlers->Append(Shared<MouseEventHandler>(h));
        return;
    }
    panic("task: unknown event handler");
}


TaskScheduler::TaskScheduler():
    runningTasks(Unique<List<Shared<Task>>>(new List<Shared<Task>>)),
    nextTaskId(7) {}

Number TaskScheduler::Start(Shared<Program> p, Unique<TaskContext> uniq_ctx) {
    Number id = nextTaskId;
    nextTaskId += 1;
    auto task = Shared<Task>(new Task(id, std::move(uniq_ctx), p->Run()));
    runningTasks->Append(task);
    auto& ctx = task->ctx;
    {
        String::Builder buf;
        buf.Write("[#"); buf.Write(String(id)); buf.Write("] ");
        buf.Write(p->Name());
        String title = buf.Collect();
        ctx->window->SetTitle(title);
        ctx->console->TaskNotifyStarted(ctx->consoleCommandNumber, id);
    }
    return id;
}

bool TaskScheduler::Kill(Number id, bool consoleDisposed) {
    bool found = false;
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->id == id) {
            found = true;
            task->status->killed = true;
            task->status->consoleDisposed = consoleDisposed;
            break;
        }
    }
    return found;
}

bool TaskScheduler::Kill(AbstractWindow *window) {
    bool found = false;
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->ctx->window == window) {
            found = true;
            task->status->killed = true;
            break;
        }
    }
    return found;
}

void TaskScheduler::Cycle(CycleInfo* info) {
    if (info == nullptr) {
        panic("task-scheduler: cycle info pointer cannot be null");
    }
    auto remaining = Unique<List<Shared<Task>>>(new List<Shared<Task>>());
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->status->killed) {
            task->ctx->window->Destroy();
            if ( !(task->status->consoleDisposed) ) {
                task->ctx->console->TaskNotifyKilled(task->ctx->consoleCommandNumber);
            }
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
    info->tasksChanged = (remaining->Length() != runningTasks->Length());
    runningTasks = std::move(remaining);
    bool somethingExecuted = false;
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        if (task->runQueue->NotEmpty()) {
            auto k = task->runQueue->Shift();
            auto result = k->Run(*(task->ctx), *(task->status));
            if (result.next != nullptr) {
                task->QueueContinuation(result.next);
            }
            if (result.newHandler != nullptr) {
                task->AttachEventHandler(result.newHandler);
            }
            somethingExecuted = true;
        }
    }
    info->somethingExecuted = somethingExecuted;
}

bool TaskScheduler::DispatchEvent(TimerEvent ev) {
    bool consumed = false;
    for (auto it = runningTasks->Iterate(); it->HasCurrent(); it->Proceed()) {
        auto task = it->Current();
        consumed = (consumed || task->ProcessEvent(ev));
    }
    return consumed;
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

