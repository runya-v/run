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

#include "base.hpp"
#include "Singleton.hpp"


namespace base {
    static const uint32_t LOG_FILE_DEPTH = 1024 * 1024;
    
    
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

        std::uint32_t _file_number;
        std::uint32_t _file_size;
        std::uint32_t _file_line_number;

        Queue _queue;
        std::mutex _mutex;
        std::condition_variable _cond;

        ThreadPtr _thread;
        bool _is_run;
        
        bool _log_out;
        bool _log_out_file;
        bool _log_file_compress;
        uint32_t _log_file_depth;

    public:
        Log();
        ~Log();

        void print(const std::string& module, const Level& level, const std::string& message);

        void start();
        void stop();
    };


    #define LOG(module, level, msg) base::Singleton<base::Log>::getInstance()->print((module), (level), (msg))
    #define MODULE typeid(*this).name()
    #define METHOD (std::string(MODULE) + "::" + __FUNCTION__)
} // namespace base
