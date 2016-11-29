/**
 * Created by Jian Chen
 * @since  2016.11.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_socket.hpp>
#include <gtest/gtest.h>

using chen::basic_socket;
using chen::invalid_handle;

TEST(BasicSocketTest, General)
{
    EXPECT_FALSE(basic_socket());
    EXPECT_FALSE(basic_socket().valid());

    EXPECT_FALSE(basic_socket(nullptr));
    EXPECT_FALSE(basic_socket(nullptr).valid());

    EXPECT_FALSE(basic_socket(invalid_handle));
    EXPECT_FALSE(basic_socket(invalid_handle, 0, 0, 0));
}