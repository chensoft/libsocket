/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/vector.hpp>
#include <gtest/gtest.h>

using namespace chen;

TEST(BaseVectorTest, General)
{
    // unique
    std::vector<int> vector({1, 2, 2, 1, 3, 4, 4, 5});
    vector::unique(vector);

    EXPECT_EQ(std::vector<int>({1, 2, 3, 4, 5}), vector);

    // union
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {3, 4, 5};
    std::vector<int> v  = vector::combine(v1, v2);

    EXPECT_EQ(std::vector<int>({1, 2, 3, 4, 5}), v);

    // intersection
    v1 = {1, 2, 3};
    v2 = {3, 4, 5};
    v  = vector::intersect(v1, v2);

    EXPECT_EQ(std::vector<int>({3}), v);

    // difference
    v1 = {1, 2, 3};
    v2 = {3, 4, 5};
    v  = vector::difference(v1, v2);

    EXPECT_EQ(std::vector<int>({1, 2}), v);
}