#include <thread>
#include <sstream>
#include <iomanip>
#include <ctime>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/lexical_cast.hpp>

#include "Log.hpp"

using namespace base;

#ifndef LOG_MAX_QUEUE_SIZE
#   define LOG_MAX_QUEUE_SIZE std::numeric_limits<std::uint8_t>::max()
#endif


namespace boost {
    void tss_cleanup_implemented(void)
    {}


    template <>
    inline std::string lexical_cast<std::string, Log::Level>(const Log::Level& value) {
        switch (value) {
            case INFO:
               return "INFO";
            case DEBUG:
               return "DEBUG";
            case WARNING:
               return "WARNING";
            case ERROR:
                return "ERROR";
            case FATAL:
                return "FATAL";
            default:
                BOOST_THROW_EXCEPTION(boost::bad_lexical_cast());
        }
    }
}


static std::string logFileName(std::uint32_t i, bool z) {
    time_t now = time(NULL);
    struct tm * ptm = localtime(&now);
    char buffer[32];
    // Format: Mo, 15.06.2009 20:20:00
    strftime(buffer, 32, "%a,%d.%m.%Y-%H:%M:%S", ptm);

    boost::format f = boost::format("%s-%u.%s") 
        % buffer 
        % i 
        % (z ? "zippedlog" : "log")
        ;
    return f.str();
}


Log::Log()
    : _file_number(0)
    , _file_size(std::numeric_limits<std::uint32_t>::max())
    , _file_line_number(0)
    , _is_run(false)
    , _log_out(false)
    , _log_out_file(true)
    , _log_file_compress(false)
    , _log_file_depth(LOG_FILE_DEPTH)
{
    start();
}


Log::~Log() {
    stop();
    std::cout << "Log is stopped.\n" << std::flush;
}


void Log::init(bool log_out, bool log_out_file, bool log_file_compress, uint32_t log_file_depth) {
    _log_out           = log_out;
    _log_out_file      = log_out_file;
    _log_file_compress = log_file_compress;
    _log_file_depth    = LOG_FILE_DEPTH;
}


void Log::execute() {
    while (true) {
        QueueTask task; 
        {
            std::unique_lock<std::mutex> lock(_mutex);

            while (_queue.empty()) {
                if (not _is_run) {
                    close();
                    return;
                }
                _cond.wait(lock);
            }
            task = _queue.front();
            _queue.pop();
        }
        auto module  = std::get<0>(task);
        auto level   = std::get<1>(task);
        auto message = std::get<2>(task);
        auto time    = std::get<3>(task);

        boost::format f = boost::format("%5u. [%s] [%s] [%s] %s\n")
            % boost::lexical_cast<std::string>(_file_line_number++)
            % boost::posix_time::to_simple_string(time)
            % boost::lexical_cast<std::string>(module)
            % boost::lexical_cast<std::string>(level)
            % message;

        if (_log_out) {
            std::cout << f.str() << std::flush;
        }

        if (_log_out_file) {
            if (_file_size >= _log_file_depth) {
                open();
            }

            if (_file.is_open()) {
                _file << f.str();
                _file_size = static_cast<std::uint32_t>(_file.tellp());
            }
        }
    }
}


void Log::handleCancel() {
    _cond.notify_one();
}


void Log::open() {
    if (_file.is_open()) {
        close();
    }
    _file.open(logFileName(_file_number, false));
}


void Log::close() {
    if (not _file.is_open()) {
        return;
    }
    _file.close();

    auto now = std::chrono::system_clock::now();

    for (
        auto it = boost::filesystem::directory_iterator(boost::filesystem::path(".")); 
        it not_eq boost::filesystem::directory_iterator(); 
        ++it) 
    {
        if (not boost::filesystem::is_regular_file(it->status())) {
            continue;
        }

        boost::filesystem::path file_path(it->path());

        if (file_path.extension() == ".log" or file_path.extension() == ".zlog") {
            auto mod = std::chrono::system_clock::from_time_t(boost::filesystem::last_write_time(it->path()));

            if (mod + std::chrono::hours(_log_file_depth) < now) {
                boost::filesystem::remove(file_path);
            }
        }
    }

    if (_log_file_compress) {
        std::ifstream ifs(logFileName(_file_number, false));
        std::ofstream ofs(logFileName(_file_number, true), std::ios_base::out | std::ios_base::binary);

        boost::iostreams::filtering_stream<boost::iostreams::output> zipper;

        zipper.push(boost::iostreams::zlib_compressor());
        zipper.push(ofs);

        boost::iostreams::copy(ifs, zipper);

        ifs.close();

        boost::filesystem::remove(boost::filesystem::path(logFileName(_file_number, false)));
    }
    ++_file_number;
}


void Log::print(const Log::Level& level, const std::string& module, const std::string& message) {
    std::unique_lock<std::mutex> lock(_mutex);

    if (_is_run) {
        auto now = boost::posix_time::microsec_clock::universal_time();

        if (_queue.size() < LOG_MAX_QUEUE_SIZE) {
            _queue.push(std::make_tuple(level, module, message, now));
        }
        else {
            boost::format f = boost::format("Log max queue size exceeded! %d messages were dropped.") % _queue.size();
            Queue q;
            _queue.swap(q);
            _queue.push(std::make_tuple(ERROR, "NONE", f.str(), now));
        }
        _cond.notify_one();
    }
}


void Log::handleException(const std::exception &e) {
    std::cerr << boost::diagnostic_information(e) << std::endl;
}


void Log::start() {
    if (_thread) {
        stop();
    }
    auto exec_func = std::bind(&Log::execute, this);
    _is_run = true;
    _thread.reset(new std::thread(exec_func));
}


void Log::stop() {
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _is_run = false;
    }
    _cond.notify_one();

    if (_thread) {
        _thread->join();
        _thread.reset();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LogSequence::Head::Next::Next(const Next &next) 
    : _stream(next._stream)
{}


LogSequence::Head::Head(const Head &head)
    : _stream(head._stream)
    , _level(head._level)
    , _module(head._module)
{}


LogSequence::LogSequence(const Log::Level &level, const std::string &module) 
    : _level(level)
    , _module(module)
{}
