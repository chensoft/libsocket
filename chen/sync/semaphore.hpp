/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <mutex>
#include <condition_variable>

namespace chen
{
    class semaphore final
    {
    public:
        semaphore() = default;
        ~semaphore() = default;

    public:
        void wake();
        void wait();

    private:
        semaphore(const semaphore&) = delete;
        semaphore& operator=(const semaphore&) = delete;

    private:
        int _count = 0;
        std::mutex _mutex;
        std::condition_variable _cond;
    };
}