/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(BaseMapTest, General)
{
    // exist
    std::map<int, int> map = {{1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10}};

    EXPECT_TRUE(chen::map::exist(map, 3));
    EXPECT_FALSE(chen::map::exist(map, 8));

    // find
    EXPECT_EQ(8, chen::map::find(map, 3));
    EXPECT_EQ(15, chen::map::find(map, 10, 15));

    // keys
    auto keys = chen::map::keys(map);
    std::sort(keys.begin(), keys.end());

    EXPECT_EQ(std::vector<int>({1, 2, 3, 4, 5}), keys);

    // values
    auto values = chen::map::values(map);
    std::sort(values.begin(), values.end());

    EXPECT_EQ(std::vector<int>({6, 7, 8, 9, 10}), values);

    // flip
    auto flip = chen::map::flip(map);
    std::map<int, int> ref({{6, 1}, {7, 2}, {8, 3}, {9, 4}, {10, 5}});

    EXPECT_EQ(ref, flip);
}