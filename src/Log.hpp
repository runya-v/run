#pragma once

#include <cstdint>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <typeinfo>
#include <tuple>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "base.hpp"
#include "Singleton.hpp"


namespace base {
    static const uint32_t LOG_FILE_DEPTH = 1024 * 1024;
    
    
    class Log {
    public:
        enum class Level {
            _info,
            _debug,
            _warning,
            _error,
            _fatal
        };

    private:
        typedef std::tuple<Level, std::string, std::string, boost::posix_time::ptime> QueueTask;
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

        void init(
            bool log_out,
            bool log_out_file,
            bool log_file_compress = false,
            uint32_t log_file_depth = LOG_FILE_DEPTH);
        void print(const Level& level, const std::string& module, const std::string& message);

        void start();
        void stop();
    };
    
    struct LogAggregator {
        std::stringstream _stream;
        Log::Level _level;
        std::string _module;

        LogAggregator(const Log::Level &level, const std::string &module) 
            : _level(level)
            , _module(module)
        {}

        template<class Type>
        LogAggregator& operator << (const Type &value) {
            _stream << value;
            return *this;
        }   
        
        ~LogAggregator() {
            Singleton<Log>::getInstance()->print(_level, _module, _stream.str().c_str());
        }
    };
} // namespace base


#define LOGM(level, method) base::LogAggregator XAFTERX(log_, __LINE__)((level), (method)); XAFTERX(log_, __LINE__)
#define LOG(level) LOGM((level), __FUNC__)

#define INFO    base::Log::Level::_info
#define DEBUG   base::Log::Level::_debug
#define WARNING base::Log::Level::_warning
#define ERROR   base::Log::Level::_error
#define FATAL   base::Log::Level::_fatal

#define METHOD (__FUNC__)

#define MODULE typeid(*this).name()
