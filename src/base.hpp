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


namespace base {
    namespace ph   = std::placeholders;
    namespace asio = boost::asio;
    namespace bs   = boost::system;
    namespace bpt  = boost::posix_time;

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

    typedef bpt::ptime Ptime;
    typedef bpt::time_duration TimeDuration;

    typedef bpt::seconds Seconds;
    typedef boost::uuids::uuid Uuid;

    typedef std::shared_ptr<std::thread> ThreadPtr;

    class ThreadPool;
    typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;
    typedef std::weak_ptr<ThreadPool>   ThreadPoolWptr;

    class Timer;
    typedef std::shared_ptr<Timer> TimerPtr;

    typedef std::function<void()>                 Handle;
    typedef std::function<void(const ErrorCode&)> AsioHandle;
} // namespace base


#define REPLACE_CRLF(str, to) boost::regex_replace((str), boost::regex("\r\n"), (to))
#define CRLF_TO_STR(str) REPLACE_CRLF((str), "<CRLF>")
