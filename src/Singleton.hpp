#pragma once

#include <memory>
#include <thread>
#include <mutex>

#include <boost/noncopyable.hpp>


namespace base {
    template<class T>
    class Singleton
        : private boost::noncopyable
    {
    public:
        static T* getInstance() {
            static std::shared_ptr<T> _only_one_instance;
            static std::mutex _mutex;

            if (not _only_one_instance.get()) {
                std::lock_guard<std::mutex> lock(_mutex);

                if (not _only_one_instance) {
                    _only_one_instance.reset(new T());
                }
            }
            return _only_one_instance.get();
        }
    };
} // namespace base
