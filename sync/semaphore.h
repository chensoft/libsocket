/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <mutex>
#include <condition_variable>

namespace chen
{
    // todo add unit test
    class semaphore final
    {
    public:
        void wake();
        void wait();

    private:
        int _count = 0;
        std::mutex _mutex;
        std::condition_variable _cond;
    };
}