/**
 * Created by Jian Chen
 * @since  2016.06.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/mt/threadpool.hpp"
#include "chen/base/num.hpp"
#include "gtest/gtest.h"

TEST(MtThreadpoolTest, Single)
{
    using chen::num;

    // check prime number, it's time consuming
    // when build in release mode, it takes about 100ms on my Macbook
    // my cpu is 2.6 GHz Intel Core i5
    for (std::size_t i = 1; i <= 20000; ++i)
        num::prime(i);
}

TEST(MtThreadpoolTest, Multiple)
{
    using chen::num;
    using chen::threadpool;

    // make sure your cpu is multi-core, otherwise the elapsed time is close to the above test
    // it takes about 50ms on my Macbook because my cpu is dual-core
    auto check = [] (std::size_t min, std::size_t max) {
        for (std::size_t i = min; i <= max; ++i)
            num::prime(i);
    };

    threadpool pool;
    std::vector<std::future<void>> task;

    // we split task into several parts
    auto count = pool.count();
    auto part  = 20000 / count;

    for (std::size_t i = 0; i < count; ++i)
    {
        task.emplace_back(pool.post(std::bind(check, 1 + part * i, part * (i + 1))));
    }

    // wait all tasks to be done
    for (auto &future : task)
        future.get();
}