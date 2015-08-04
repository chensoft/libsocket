/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "threadpool.h"

using namespace chen;

// ------------------------------------------------------------------
// threadpool
#pragma mark -
#pragma mark threadpool

threadpool::threadpool(unsigned int count)
{
    // assign thread count according to cpu count
    if (!count)
        count = std::max(1u, std::thread::hardware_concurrency());

    for (auto i = 0; i < count; ++i)
        this->_pool.push_back(std::thread(std::bind(&threadpool::run, this)));
}

threadpool::~threadpool()
{
    // release thread
    this->_destroy = true;
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
    while (!this->_destroy)
    {
        // check job
        job_type job;

        {
            std::lock_guard<std::mutex> lock(this->_mutex);

            if (this->_queue.size())
            {
                job = this->_queue.front();
                this->_queue.erase(this->_queue.begin());
            }
        }

        // run job
        if (job)
            job();

        // sleep
        this->_semaphore.wait();
    }
}