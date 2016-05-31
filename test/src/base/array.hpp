/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(BaseArrayTest, General)
{
    // unique
    std::vector<int> vector({1, 2, 2, 1, 3, 4, 4, 5});
    chen::array::unique(vector);

    EXPECT_EQ(std::vector<int>({1, 2, 3, 4, 5}), vector);

    // union
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {3, 4, 5};
    std::vector<int> v  = chen::array::combine(v1, v2);

    EXPECT_EQ(std::vector<int>({1, 2, 3, 4, 5}), v);

    // intersection
    v1 = {1, 2, 3};
    v2 = {3, 4, 5};
    v  = chen::array::intersect(v1, v2);

    EXPECT_EQ(std::vector<int>({3}), v);

    // difference
    v1 = {1, 2, 3};
    v2 = {3, 4, 5};
    v  = chen::array::difference(v1, v2);

    EXPECT_EQ(std::vector<int>({1, 2}), v);
}