/**
 * Created by Jian Chen
 * @since  2016.06.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(MtThreadpoolTest, Single)
{
    // check prime number, it's time consuming
    // when build in release mode, it takes about 100ms on my Macbook
    // my cpu is 2.6 GHz Intel Core i5
    for (unsigned i = 1; i <= 20000; ++i)
        chen::num::prime(i);
}

TEST(MtThreadpoolTest, Multiple)
{
    // make sure your cpu is multi-core, otherwise the elapsed time is close to the above test
    // it takes about 50ms on my Macbook because my cpu is dual-core
    auto check = [] (unsigned min, unsigned max) {
        for (unsigned i = min; i <= max; ++i)
            chen::num::prime(i);
    };

    chen::threadpool pool;
    std::vector<std::future<void>> task;

    // we split task into 4 parts
    auto part = 20000 / 4;

    for (int i = 0; i < 4; ++i)
    {
        task.push_back(pool.post(std::bind(check, 1 + part * i, part * (i + 1))));
    }

    // wait all tasks to be done
    for (auto &future : task)
        future.get();
}