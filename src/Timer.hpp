#pragma once


namespace base {
    
    class Timer {
    public:
        template<class Callable, class... Arguments>
        Timer(int after, bool async, Callable &&f, Arguments&&... args) {
            typedef std::function<typename std::result_of<Callable(Arguments...)>::type()> TimerTask;

            TimerTask task(std::bind(std::forward<Callable>(f), std::forward<Arguments>(args)...));

            if (async) {
                auto timer_run = [after, task]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(after));
                    task();
                };

                ThreadPtr timer_thread(new std::thread(timer_run));
                timer_thread->detach();
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
            }
        }
    };
} // namespace base
