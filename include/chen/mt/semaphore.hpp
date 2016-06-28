/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @thread-safe
 */
#pragma once

#include <condition_variable>
#include <mutex>

namespace chen
{
    class semaphore final
    {
    public:
        explicit semaphore(std::size_t count = 0);

    public:
        void post();
        void wait();

    private:
        semaphore(const semaphore&) = delete;
        semaphore& operator=(const semaphore&) = delete;

    private:
        std::size_t _count = 0;
        std::mutex _mutex;
        std::condition_variable _cond;
    };
}