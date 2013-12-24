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
    
    
    struct LogSequence {
        struct Head {
            struct Next {
                std::stringstream *_stream;

                template<class Type>
                explicit Next(std::stringstream *stream, const Type &value) 
                    : _stream(stream)
                {
                    (*_stream) << value;
                }

                Next(const Next &next);

                template<class Type>
                Next operator << (const Type &value) {
                    return Next(_stream, value);
                }
            };

            std::shared_ptr<std::stringstream> _stream;
            Log::Level _level;
            std::string _module;
          
            template<class Type>
            explicit Head(const Log::Level &level, const std::string& module, const Type &value) 
                : _stream(new std::stringstream)
                , _level(level)
                , _module(module)
            {
                (*_stream) << value;
            }

            Head(const Head &head);
            
            ~Head() {
                base::Singleton<base::Log>::getInstance()->print(_level, _module, _stream->str().c_str());
            }

            template<class Type>
            Next operator << (const Type &value) {
                return Next(_stream.get(), value);
            }
        };
        
        Log::Level _level;
        std::string _module;

        LogSequence(const Log::Level &level, const std::string &module);

        template<class Type>
        Head operator << (const Type &value) {
            return Head(_level, _module, value);
        }
    };
} // namespace base


#define LOG_TO_STDOUT base::Singleton<base::Log>::getInstance()->init(true, false);

#define LOGM(level, method) base::LogSequence XAFTERX(log_, __LINE__)((level), (method)); XAFTERX(log_, __LINE__)
#define LOG(level) LOGM((level), __FUNC__)

#define INFO    base::Log::Level::_info
#define DEBUG   base::Log::Level::_debug
#define WARNING base::Log::Level::_warning
#define ERROR   base::Log::Level::_error
#define FATAL   base::Log::Level::_fatal

#define METHOD (__FUNC__)

#define MODULE typeid(*this).name()
