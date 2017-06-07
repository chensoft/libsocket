/**
 * Created by Jian Chen
 * @since  2017.06.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/time/date.hpp"
#include "gtest/gtest.h"

TEST(TimeDateTest, General)
{
    chen::date d;

    EXPECT_GE(d.microsecond, 0);
    EXPECT_LE(d.microsecond, 999999);
    EXPECT_GE(d.second, 0);
    EXPECT_LE(d.second, 59);
    EXPECT_GE(d.minute, 0);
    EXPECT_LE(d.minute, 59);
    EXPECT_GE(d.hour, 0);
    EXPECT_LE(d.hour, 23);
    EXPECT_GE(d.day, 1);
    EXPECT_LE(d.day, 31);
    EXPECT_GE(d.wday, 0);
    EXPECT_LE(d.wday, 6);
    EXPECT_GE(d.month, 1);
    EXPECT_LE(d.month, 12);
    EXPECT_GE(d.year, 2017);
    EXPECT_GE(d.zone, 0);
}