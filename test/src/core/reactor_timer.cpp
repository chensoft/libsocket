/**
 * Created by Jian Chen
 * @since  2017.01.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/core/reactor.hpp>
#include <gtest/gtest.h>

using chen::timer;
using chen::reactor;

TEST(CoreReactorTest, Timer)
{
    int c1 = 0, c2 = 0, c3 = 0;

    timer t1;
    t1.timeout(std::chrono::milliseconds(10));

    timer t2;
    t2.future(std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(20));

    timer t3;
    t3.interval(std::chrono::milliseconds(30));

    reactor r;

    r.set(&t1, [&] () {
        ++c1;
    });

    r.set(&t2, [&] () {
        ++c2;
    });

    r.set(&t3, [&] () {
        if (++c3 == 5)
            r.stop();
    });

    r.run();

    EXPECT_EQ(1, c1);
    EXPECT_EQ(1, c2);
    EXPECT_EQ(5, c3);
}