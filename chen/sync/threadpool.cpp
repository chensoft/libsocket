/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "threadpool.hpp"

using namespace chen;

// ------------------------------------------------------------------
// threadpool
threadpool::threadpool(std::size_t count)
{
    // assign thread count according to cpu count
    if (!count)
        count = std::max(1u, std::thread::hardware_concurrency());

    for (std::size_t i = 0; i < count; ++i)
        this->_pool.push_back(std::thread(std::bind(&threadpool::run, this)));
}

threadpool::~threadpool()
{
    // release thread
    this->_destroy = true;

    for (std::size_t i = 0, len = this->_pool.size(); i < len; ++i)
        this->_semaphore.wake();

    for (auto &thread : this->_pool)
        thread.join();
}

void threadpool::async(const job_type &job)
{
    {
        std::lock_guard<std::mutex> lock(this->_mutex);
        this->_queue.push_back(job);
    }

    this->_semaphore.wake();
}

void threadpool::run()
{
    while (true)
    {
        // check job
        bool quit = false;
        std::size_t size = 0;
        job_type job;

        {
            std::lock_guard<std::mutex> lock(this->_mutex);

            size = this->_queue.size();

            if (size)
            {
                job = this->_queue.front();
                this->_queue.erase(this->_queue.begin());
            }

            if ((size <= 1) && this->_destroy)
                quit = true;
        }

        // run job
        if (job)
            job();

        // quit loop
        if (quit)
            break;

        // sleep
        if (size <= 1)
            this->_semaphore.wait();
    }
}