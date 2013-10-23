#ifndef BOOST_STATIC_LINK
#   define BOOST_TEST_DYN_LINK
#endif // BOOST_STATIC_LINK

#define BOOST_TEST_MODULE ForexPrj
#define BOOST_AUTO_TEST_MAIN

#define THREAD_POOL_STATISTICS

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/format.hpp>

#include <cmath>
#include <atomic>
#include <functional>
#include <ctime>

#include "ThreadPool.hpp"
#include "Task.hpp"


struct UnitWork
{
    std::time_t _tm;
    std::string _time;

    UnitWork()
    {
        std::time_t now = time(NULL);
        struct tm *ptm = localtime(&now);
        char buf[32];
        strftime(buf, sizeof(buf), "%H:%M:%S %d.%m.%Y", ptm);

        _time = buf;
        _tm = base::castToTimeT(buf);
    }

    UnitWork& operator= (const UnitWork &work)
    {
        if (this not_eq &work)
        {
            _tm   = work._tm;
            _time = work._time;
        }
        return *this;
    }
};


static const std::uint64_t CALC_ITERATIONS = 1000;
static const std::uint64_t SECOND          = 1000000; // by `mks`


struct WorkTask
{
    std::size_t _work_iterations;

    WorkTask(std::size_t work_iterations)
        : _work_iterations(work_iterations)
    {}

    void operator() ()
    {
        for (std::size_t i = 0; i < _work_iterations; ++i)
        {
            UnitWork();
        }
    }
};


struct CalcWorkUnitTime
{
    std::uint64_t _unit_time;

    CalcWorkUnitTime()
    {
        boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
        for (std::uint64_t i = 0; i < CALC_ITERATIONS; i++)
        {
            WorkTask wt(1);
            wt();
        }
        boost::posix_time::time_duration dt = boost::posix_time::microsec_clock::local_time() - start_time;
        _unit_time = std::ceil((double)dt.total_microseconds() / (double)CALC_ITERATIONS);

        boost::format f = boost::format("Unit work time: %d mks;")
            % _unit_time
            ;
        LOG(METHOD, base::Log::Level::DEBUG, f.str());
    }

    std::uint64_t getIterations(std::uint64_t need_time)
    {
        if (not need_time)
        {
            return 1;
        }
        return std::floor((double)need_time / (double)_unit_time);
    }

    std::uint64_t getFrequence()
    {
        return getIterations(SECOND);
    }
};


class Worker
{
public:
    CalcWorkUnitTime _calc_unit_time;
    std::shared_ptr<base::ThreadPool> _pool;
    std::atomic_bool _stoped;
    std::uint64_t _task_time;
    std::atomic_uint_fast64_t _works_by_task;
    std::atomic_uint_fast64_t _task_time_sum;
    std::atomic_uint_fast64_t _sub_tasks;
    std::atomic_uint_fast64_t _max_tasks;
    base::TaskPtr _task_timer;

    Worker(std::uint32_t timer, std::uint32_t task_time, std::uint64_t threads_num)
        : _pool(std::make_shared<base::ThreadPool>(threads_num))
        , _task_time(task_time)
        , _works_by_task(_calc_unit_time.getIterations(_task_time))
        , _task_time_sum(0)
        , _sub_tasks(0)
        , _max_tasks(0)
    {
        boost::format f = boost::format("Start:\n"
                                        "\ttime interval = %d;\n"
                                        "\ttask time     = %d;\n"
                                        "\tworks by task = %d;\n"
                                        "\tworks freq    = %d")
            % timer
            % _task_time
            % _works_by_task
            % _calc_unit_time.getFrequence()
            ;
        LOG(METHOD, base::Log::Level::DEBUG, f.str());

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        auto exec_handle = std::bind(&Worker::handleExecute, this, base::ph::_1);
        boost::posix_time::ptime start_time(boost::posix_time::microsec_clock::local_time());
        std::uint64_t task_count = 0;
        //_task_timer = std::make_shared<base::Task>((base::TaskHandle&&)exec_handle);

        do
        {
            base::TaskPtr hello_task = std::make_shared<base::Task>((base::TaskHandle&&)exec_handle);
            _pool->enqueue(std::bind(&base::Task::execute,  hello_task.get(), hello_task));
            ++task_count;
            //LOG(METHOD, base::Log::Level::DEBUG, boost::lexical_cast<std::string>(_task_timer->getTimeInterval()));
            //_task_timer = std::make_shared<base::Task>((base::TaskHandle&&)exec_handle);
        }
        while (((boost::posix_time::microsec_clock::local_time() - start_time).total_microseconds()) < timer);

        // Complete
        f = boost::format("Generate statistik:\n"
                          "\tmidle task time = %d;\n"
                          "\tmax duty cycle  = %d;")
            % (_task_time_sum / task_count)
            % (std::uint64_t)std::floor((double)(task_count * SECOND) / (double)timer)
            ;
        LOG(METHOD, base::Log::Level::DEBUG, f.str());
        _pool.reset();
    }



    void handleExecute(const base::TaskPtr &task)
    {
        // Execute task
        std::uint64_t init_dt = task->getTimeInterval();
        std::uint64_t wbt = _works_by_task;
        wbt = (not wbt) ? 1 : wbt;
        WorkTask wt(wbt);
        wt();

        // Statistics
        std::uint64_t exe_dt = task->getTimeInterval() - init_dt;
        _task_time_sum += exe_dt;
        _works_by_task = std::floor((double)(_task_time * wbt) / ((double)exe_dt));

        boost::format f = boost::format("ws = %d; init dt = %d; exe dt = %d; total = %d; sum = %d")
            % wbt
            % init_dt
            % exe_dt
            % (init_dt + exe_dt)
            % _task_time_sum
            ;
        LOG(METHOD, base::Log::Level::DEBUG, f.str());
    }
};


static const uint32_t TIME_INTERVAL = 60000000; // by `mks`
static const uint32_t TASK_DELAY    = 1000;     // `mks`
static const uint32_t THREADS_NUM   = 4;


BOOST_AUTO_TEST_CASE(TestPerformance)
{
    Worker(TIME_INTERVAL, TASK_DELAY, THREADS_NUM);
}

