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


static std::string logFileName(std::uint32_t i, bool z) {
    time_t now = time(NULL);
    struct tm * ptm = localtime(&now);
    char buffer[32];
    // Format: Mo, 15.06.2009 20:20:00
    strftime(buffer, 32, "%a,%d.%m.%Y-%H:%M:%S", ptm);

    boost::format f = boost::format("%s-%u.%s") 
        % buffer 
        % i 
        % (z ? "zippedlog" : "log");
    return f.str();
}


namespace boost {
    void tss_cleanup_implemented(void)
    {}


    template <>
    inline std::string lexical_cast<std::string, Log::Level>(const Log::Level& value) {
        switch (value) {
            case Log::Level::NONE:
                return "NONE";
            case Log::Level::INFO:
               return "INFO";
            case Log::Level::DEBUG:
               return "DEBUG";
            case Log::Level::WARNING:
               return "WARNING";
            case Log::Level::ERROR:
                return "ERROR";
            case Log::Level::FATAL:
                return "FATAL";
            default:
                BOOST_THROW_EXCEPTION(boost::bad_lexical_cast());
        }
    }
}


Log::Log()
    : _fileNumber(0)
    , _fileSize(std::numeric_limits<std::uint32_t>::max())
    , _fileLineNumber(0)
    , _is_run(false)
{
    _config.set("Log.log-out", true);
    _config.set("Log.log-out-file", true);
    _config.set("Log.log-compress", false);
    _config.set("Log.log-depth", 1);
    start();
}


Log::Log(base::ConfigDepot &config)
    : _fileNumber(0)
    , _fileSize(std::numeric_limits<std::uint32_t>::max())
    , _fileLineNumber(0)
    , _is_run(false)
{
    if (not config.isSection()) {
        _config = config.unload("Log");
    }
    else {
        _config = config;
    }
}


Log::~Log() {
    stop();
    std::cout << "Log is stopped.\n" << std::flush;
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
        auto module  = task.get<0>();
        auto level   = task.get<1>();
        auto message = task.get<2>();
        auto time    = task.get<3>();

        boost::format f = boost::format("%5u. [%s] [%s] [%s] %s\n")
            % boost::lexical_cast<std::string>(_fileLineNumber++)
            % boost::posix_time::to_simple_string(time)
            % boost::lexical_cast<std::string>(module)
            % boost::lexical_cast<std::string>(level)
            % message;

        if (_config.get<bool>("log-out", true)) {
            std::cout << f.str() << std::flush;
        }

        if (_config.get<bool>("log-out-file", false)) {
            if (_fileSize >= _config.get<std::uint32_t>("log-out-file", 1024 * 1024)) {
                open();
            }

            if (_file.is_open()) {
                _file << f.str();
                _fileSize = static_cast<std::uint32_t>(_file.tellp());
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
    _file.open(logFileName(_fileNumber, false));
}


void Log::close() {
    if (not _file.is_open()) {
        return;
    }
    _file.close();

    auto now = std::chrono::system_clock::now();

    for (auto it = boost::filesystem::directory_iterator(boost::filesystem::path(".")); it != boost::filesystem::directory_iterator(); ++it) {
        if (not boost::filesystem::is_regular_file(it->status())) {
            continue;
        }

        boost::filesystem::path p(it->path());

        if (p.extension() == ".log" || p.extension() == ".zippedlog") {
            auto mod = std::chrono::system_clock::from_time_t(boost::filesystem::last_write_time(it->path()));

            if (mod + std::chrono::hours(_config.get<std::uint32_t>("log-depth", 30 * 24)) < now) {
                boost::filesystem::remove(p);
            }
        }
    }

    if (_config.get<bool>("log-compress",false)) {
        std::ifstream ifs(logFileName(_fileNumber, false));
        std::ofstream ofs(logFileName(_fileNumber, true), std::ios_base::out | std::ios_base::binary);

        boost::iostreams::filtering_stream<boost::iostreams::output> zipper;

        zipper.push(boost::iostreams::zlib_compressor());
        zipper.push(ofs);

        boost::iostreams::copy(ifs, zipper);

        ifs.close();

        boost::filesystem::remove(boost::filesystem::path(logFileName(_fileNumber, false)));
    }

    _fileNumber++;
}


void Log::print(const std::string& module, const Log::Level& level, const std::string& message) {
    std::unique_lock<std::mutex> lock(_mutex);

    if (_is_run) {
        auto now = boost::posix_time::microsec_clock::universal_time();

        if (_queue.size() < LOG_MAX_QUEUE_SIZE) {
            _queue.push(boost::make_tuple(module, level, message, now));
        }
        else {
            boost::format f = boost::format("Log max queue size exceeded! %d messages were dropped.") % _queue.size();
            Queue q;
            _queue.swap(q);
            _queue.push(boost::make_tuple("NONE", Log::Level::ERROR, f.str(), now));
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
