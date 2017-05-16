/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "socket/inet/inet_adapter.hpp"
#include "gtest/gtest.h"

using chen::inet_adapter;

TEST(InetAdapterTest, General)
{
    // the following codes have no practical meaning
    // because adapters are different on each system
    // we can't do unit test for this class
    std::uint8_t addr[16]{};

    EXPECT_TRUE(!inet_adapter::enumerate().empty());
    EXPECT_NO_THROW(inet_adapter::scope(addr, "0"));
    EXPECT_NO_THROW(inet_adapter::scope(addr, "lo0"));
    EXPECT_NO_THROW(inet_adapter::scope(0));
}