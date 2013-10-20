#pragma once

#include <cstdint>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <typeinfo>

#include <boost/fusion/include/boost_tuple.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Singleton.hpp"
#include "Service.hpp"
#include "ConfigDepot.hpp"


namespace base {
    
    class Log {
    public:
        enum class Level {
            NONE,
            INFO,
            DEBUG,
            WARNING,
            ERROR,
            FATAL
        };

    private:
        typedef boost::tuple<std::string, Level, std::string, boost::posix_time::ptime> QueueTask;
        typedef std::queue<QueueTask> Queue;

        virtual void execute();
        virtual void handleCancel();
        virtual void handleException(const std::exception& e);

        virtual void onStop()
        {}

        void open();
        void close();

        std::ofstream _file;

        std::uint32_t _fileNumber;
        std::uint32_t _fileSize;
        std::uint32_t _fileLineNumber;

        Queue _queue;
        std::mutex _mutex;
        std::condition_variable _cond;

        ConfigDepot _config;
        ThreadPtr _thread;
        bool _is_run;

    public:
        Log();
        explicit Log(base::ConfigDepot& config);
        ~Log();

        void setConfig(base::ConfigDepot& config);

        void print(const std::string& module, const Level& level, const std::string& message);

        void start();
        void stop();
    };


    #define LOG(module, level, msg) base::Singleton<base::Log>::getInstance()->print((module), (level), (msg))
    #define MODULE typeid(*this).name()
    #define METHOD (std::string(MODULE) + "::" + __FUNCTION__)
} // namespace base
