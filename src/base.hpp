#pragma once

#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#define BOOST_THREAD_USE_LIB

extern "C" {
   namespace boost {
        /*
         * This function's sole purpose is to cause a link error in cases where
         * automatic tss cleanup is not implemented by Boost.Threads as a
         * reminder that user code is responsible for calling the necessary
         * functions at the appropriate times (and for implementing an a
         * tss_cleanup_implemented() function to eliminate the linker's
         * missing symbol error).
         *
         * If Boost.Threads later implements automatic tss cleanup in cases
         * where it currently doesn't (which is the plan), the duplicate
         * symbol error will warn the user that their custom solution is no
         * longer needed and can be removed.
         */
        void tss_cleanup_implemented(void);
   }
}

#include <boost/thread.hpp>

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>


namespace base {
    namespace ph   = std::placeholders;
    namespace asio = boost::asio;
    namespace bs   = boost::system;
    namespace bpt  = boost::posix_time;

    class Task;
    class Timer;
    class ThreadPool;

    typedef asio::signal_set     SignalSet;
    typedef asio::io_service     IoService;
    typedef IoService::work      Work;
    typedef IoService::strand    Strand;
    typedef asio::ip::tcp        Tcp;
    typedef Tcp::socket          Socket;
    typedef Tcp::resolver        Resolver;
    typedef Resolver::query      Query;
    typedef Resolver::iterator   ResolverIter;
    typedef Tcp::endpoint        Endpoint;
    typedef asio::deadline_timer DeadLineTimer;

    typedef std::shared_ptr<Work>          WorkPtr;
    typedef std::shared_ptr<DeadLineTimer> DeadLineTimerPtr;

    typedef std::size_t(IoService::*IoServiceRunFunc)();

    typedef bs::error_code   ErrorCode;
    typedef bs::system_error SystemError;

    typedef bpt::ptime             Ptime;
    typedef bpt::time_duration     TimeDuration;
    typedef bpt::seconds           Seconds;
    typedef boost::uuids::uuid     Uuid;
    typedef std::shared_ptr<Timer> TimerPtr;

    typedef std::shared_ptr<std::thread> ThreadPtr;
    typedef std::shared_ptr<Task>        TaskPtr;
    typedef std::shared_ptr<ThreadPool>  ThreadPoolPtr;
    typedef std::weak_ptr<ThreadPool>    ThreadPoolWptr;

    typedef std::function<void()>                 Handle;
    typedef std::function<void(const ErrorCode&)> AsioHandle;
    typedef std::function<void(const TaskPtr&)>   TaskHandle;
    
    inline std::time_t castToTimeT(const std::string &time_str)
    {
        struct tm time_tm = {};
        time_tm.tm_hour = boost::lexical_cast<int>(time_str.substr(0, 2));         //int hours since midnight     00-23
        time_tm.tm_min  = boost::lexical_cast<int>(time_str.substr(3, 2));         //int minutes after the hour   00-59
        time_tm.tm_sec  = boost::lexical_cast<int>(time_str.substr(6, 2));         //int seconds after the minute 00-60
        time_tm.tm_mday = boost::lexical_cast<int>(time_str.substr(9, 2));         //int day of the month         01-31
        time_tm.tm_mon  = boost::lexical_cast<int>(time_str.substr(12, 2)) - 1;    //int months since January     00-11
        time_tm.tm_year = boost::lexical_cast<int>(time_str.substr(15, 4)) - 1900; //int years since              1900
        return mktime(&time_tm);
    }
} // namespace base


#define REPLACE_CRLF(str, to) boost::regex_replace((str), boost::regex("\r\n"), (to))
#define CRLF_TO_STR(str) REPLACE_CRLF((str), "<CRLF>")
