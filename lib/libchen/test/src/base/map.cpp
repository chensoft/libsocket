/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/base/map.hpp"
#include "gtest/gtest.h"

TEST(BaseMapTest, General)
{
    using chen::map;

    // exist
    std::map<int, int> m = {{1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10}};

    EXPECT_TRUE(map::exist(m, 3));
    EXPECT_FALSE(map::exist(m, 8));

    // find
    EXPECT_EQ(8, map::find(m, 3));
    EXPECT_EQ(15, map::find(m, 10, 15));

    // write
    int out = 0;

    map::write(m, 10, out);
    EXPECT_EQ(0, out);

    map::write(m, 3, out);
    EXPECT_EQ(8, out);

    // keys
    auto keys = map::keys(m);
    std::sort(keys.begin(), keys.end());

    EXPECT_EQ(std::vector<int>({1, 2, 3, 4, 5}), keys);

    // values
    auto values = map::values(m);
    std::sort(values.begin(), values.end());

    EXPECT_EQ(std::vector<int>({6, 7, 8, 9, 10}), values);

    // flip
    auto flip = map::flip(m);
    std::map<int, int> ref({{6, 1}, {7, 2}, {8, 3}, {9, 4}, {10, 5}});

    EXPECT_EQ(ref, flip);
}