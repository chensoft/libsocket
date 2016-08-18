/**
 * Created by Jian Chen
 * @since  2016.05.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/mt/threadpool.hpp>

// ------------------------------------------------------------------
// threadpool
chen::threadpool::threadpool(std::size_t count) : _exit(false)
{
    if (!count)
        count = std::max(1u, std::thread::hardware_concurrency());

    try
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            std::thread thread(&threadpool::run, this);
            this->_pool.emplace_back(std::move(thread));
        }
    }
    catch (...)
    {
        this->clean();
        throw;
    }
}

chen::threadpool::~threadpool()
{
    this->clean();
}

std::size_t chen::threadpool::count() const
{
    return this->_pool.size();
}

void chen::threadpool::run()
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

void chen::threadpool::clean()
{
    this->_exit = true;

    for (std::size_t i = 0, len = this->_pool.size(); i < len; ++i)
        this->_semaphore.post();

    for (std::thread &thread : this->_pool)
    {
        if (thread.joinable())
            thread.join();
    }
}