/**
 * Created by Jian Chen
 * @since  2016.05.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "threadpool.hpp"
#include <algorithm>

using namespace chen;

// ------------------------------------------------------------------
// threadpool
threadpool::threadpool(std::size_t count)
: _exit(false)
{
    if (!count)
        count = std::max(1u, std::thread::hardware_concurrency());

    try
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            std::thread thread(&threadpool::run, this);
            this->_pool.push_back(std::move(thread));
        }
    }
    catch (...)
    {
        this->clean();
        throw;
    }
}

threadpool::~threadpool()
{
    this->clean();
}

void threadpool::run()
{
    while (true)
    {
        this->_semaphore.wait();

        if (this->_exit)
            break;

        std::unique_lock<std::mutex> lock(this->_mutex);

        if (!this->_queue.empty())
        {
            auto task = std::move(this->_queue.front());
            this->_queue.pop();
            lock.unlock();

            task();
        }
    }
}

void threadpool::clean()
{
    this->_exit = true;

    for (std::size_t i = 0, len = this->_pool.size(); i < len; ++i)
        this->_semaphore.post();

    std::for_each(this->_pool.begin(), this->_pool.end(), [] (std::thread &thread) {
        if (thread.joinable())
            thread.join();
    });
}