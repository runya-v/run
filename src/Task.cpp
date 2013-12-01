#include <exception>

#include "Task.hpp"
#include "Log.hpp"

using namespace base;


Task::Task(TaskHandle &&handle_exec)
    : _create_time(HighResolutionClock::now())
    , _task_handle_exec(handle_exec)
    , _handle_exec([](){})
    , _handle_next([](){})
{}


Task::Task(Handle &&handle_exec)
    : _create_time(HighResolutionClock::now())
    , _task_handle_exec([](const TaskPtr&){})
    , _handle_exec(handle_exec)
    , _handle_next([](){})
{}


Task::~Task()
{}


void Task::setNextTaskHandle(Handle &&handle_next) {
    _handle_next = handle_next;
}


void Task::execute(const TaskPtr &task) {
    try {
        _task_handle_exec(task);
        _handle_exec();
        _handle_next();
    }
    catch (const std::exception &e) {
        LOG(ERROR) << e.what();
    }
}


std::string& Task::getData() {
    return _data;
}


std::uint64_t Task::getTimeInterval() {
    return chr::duration_cast<Micro>(_create_time - HighResolutionClock::now()).count();
}
