#include <sstream>

#include <boost/format.hpp>

#include "ThreadPool.hpp"
#include "Log.hpp"

using namespace base;


ThreadPool::ThreadPool(size_t threads)
    : _stop(false)
    , _threads(threads)
    , _gen_tasks(0)
    , _execd_tasks(0)
    , _max_tasks(0)
{
    if (not threads)  {
        threads = 1;
        LOG(METHOD, base::Log::Level::DEBUG, "Set num threads to default.");
    }

    for (std::size_t i = 0; i < threads; ++i) {
        _execd_tasks_counts.push_back(std::make_shared<std::atomic_uint_fast64_t>(0));
    }

    for (std::size_t i = 0; i < threads; ++i) {
        std::atomic_uint_fast64_t *p_execd_tasks = _execd_tasks_counts[i].get();
        _workers.emplace_back( [this, p_execd_tasks] {
            while (true) {
                std::unique_lock<std::mutex> lock(_queue_mutex);

                while (not _stop and _tasks.empty()) {
                    _condition.wait(lock);
                }

                if (_stop and _tasks.empty()) {
                    return;
                }

                std::function<void()> task(_tasks.front());
                _tasks.pop();
                lock.unlock();

                // Execute task
                task();

                if (_max_tasks < (_gen_tasks - _execd_tasks)) {
                    _max_tasks = _gen_tasks - _execd_tasks;
                }

                ++_execd_tasks;
                ++*p_execd_tasks;
            }
        } );
    }
}


ThreadPool::~ThreadPool() {
    _stop = true;
    _condition.notify_all();

    std::stringstream counts_ss;

    for(size_t i = 0; i < _workers.size(); ++i) {
        _workers[i].join();
        counts_ss << "\n\tthread[" << i << "] = " << _execd_tasks_counts[i].get()[0] << ";";
    }

    boost::format f = boost::format("Pool statistik:\n"
                                    "\tloaded tasks   = %d;\n"
                                    "\texecd tasks    = %d;\n"
                                    "\tmax work tasks = %d;"
                                    "%s")
        % _gen_tasks
        % _execd_tasks
        % _max_tasks
        % counts_ss.str()
        ;
    LOG(METHOD, base::Log::Level::DEBUG, f.str());
}


bool ThreadPool::trySleep() {
    if ((_threads * 10) < (_gen_tasks - _execd_tasks)) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        return true;
    }
    return false;
}
