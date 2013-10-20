#pragma once

#include <vector>
#include <atomic>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "Task.hpp"


namespace base {
    
    class ThreadPool {
        std::vector<std::thread>          _workers;
        std::queue<std::function<void()>> _tasks;

        std::mutex              _queue_mutex;
        std::condition_variable _condition;
        std::atomic_bool        _stop;

        std::atomic_uint_fast64_t _threads;
        std::atomic_uint_fast64_t _gen_tasks;
        std::atomic_uint_fast64_t _execd_tasks;
        std::atomic_uint_fast64_t _max_tasks;

        std::vector<std::shared_ptr<std::atomic_uint_fast64_t>> _execd_tasks_counts;

    public:
        template<class Func, class... Args>
        auto enqueue(Func &&f, Args&&... args)
            -> std::future<typename std::result_of<Func(Args...)>::type>  
        {
            typedef typename std::result_of<Func(Args...)>::type return_type;

            ++_gen_tasks;

            if (_stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
            std::future<return_type> res = task->get_future();

            {
                std::unique_lock<std::mutex> lock(_queue_mutex);
                _tasks.push( [task]() {
                    (*task)();
                } );
            }
            _condition.notify_one();
            return res;
        }

        ThreadPool(size_t threads = std::thread::hardware_concurrency());
        ~ThreadPool();
    };
} // namespace base
