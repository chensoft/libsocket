/**
 * Created by Jian Chen
 * @since  2016.07.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/base/iterator.hpp"
#include "gtest/gtest.h"
#include <array>
#include <list>

TEST(BaseIterTest, General)
{
    using chen::input_iterator;
    using chen::forward_iterator;
    using chen::bidirectional_iterator;
    using chen::random_iterator;

    // plain text
    char buf[] = "chen";
    random_iterator<char> iter(buf);

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

    random_iterator<char> proxy(iter++);
    EXPECT_EQ('c', *proxy);

    random_iterator<char> other(iter);

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

    input_iterator<char> cur(vector.begin());
    input_iterator<char> end(vector.end());

    std::copy(cur, end, array.begin());
    EXPECT_TRUE(std::equal(vector.begin(), vector.end(), array.begin()));

    // list, its iterator is a bidirectional iterator
    std::list<char> list{'c', 'h', 'e', 'n'};

    bidirectional_iterator<char> a(list.begin());
    bidirectional_iterator<char> b(list.end());

    forward_iterator<char> f(buf);

    while (a != b)
        EXPECT_TRUE(*a++ == *f++);
}