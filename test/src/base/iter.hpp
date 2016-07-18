/**
 * Created by Jian Chen
 * @since  2016.07.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>
#include <array>

TEST(BaseIterTest, General)
{
    // plain text
    char buf[] = "chen";
    chen::iter<char> iter(buf);

    EXPECT_EQ('c', *iter);
    EXPECT_EQ('c', *iter++);
    EXPECT_EQ('h', *iter);
    EXPECT_EQ('e', *++iter);
    EXPECT_EQ('h', *--iter);
    EXPECT_EQ('h', *iter);
    EXPECT_EQ('h', *iter--);
    EXPECT_EQ('c', *iter);

    EXPECT_EQ('c', iter[0]);
    EXPECT_EQ('n', iter[3]);

    EXPECT_EQ('n', *(iter + 3));

    iter += 3;
    EXPECT_EQ('n', *iter);

    EXPECT_EQ('c', *(iter - 3));

    iter -= 3;
    EXPECT_EQ('c', *iter);

    chen::iter<char> other(iter);

    EXPECT_TRUE(iter == other);
    EXPECT_TRUE(iter <= other);
    EXPECT_FALSE(iter != other);

    ++other;
    EXPECT_TRUE(iter < other);
    EXPECT_TRUE(iter <= other);
    EXPECT_FALSE(iter > other);
    EXPECT_FALSE(iter >= other);

    // vector and array
    std::vector<char> vector(buf, buf + ::strlen(buf));
    std::array<char, 4> array{};

    chen::iter<char> cur(vector.begin());
    chen::iter<char> end(vector.end());

    std::copy(cur, end, array.begin());
    EXPECT_TRUE(std::equal(vector.begin(), vector.end(), array.begin()));
}