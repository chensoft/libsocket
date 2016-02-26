/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <list>
#include "semaphore.hpp"

namespace chen
{
    // todo add unit test
    // todo maybe use template in future
    class threadpool final
    {
    public:
        typedef std::function<void ()> job_type;

    public:
        threadpool(std::size_t count = 0);
        ~threadpool();

    public:
        // todo maybe return std::future in future
        /**
         * Put a job into queue
         */
        void async(const job_type &job);

    protected:
        void run();

    private:
        threadpool(const threadpool&) = delete;
        threadpool& operator=(const threadpool&) = delete;

    private:
        bool _destroy = false;  // is wait for destroy

        std::mutex _mutex;           // queue lock
        std::list<job_type> _queue;  // queue job

        std::vector<std::thread> _pool;  // thread
        chen::semaphore _semaphore;      // semaphore
    };
}