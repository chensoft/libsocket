/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_interface.hpp>
#include <gtest/gtest.h>

TEST(IPInterfaceTest, General)
{
    // todo
    auto list = chen::ip::interface::enumerate();

    for (auto x : list)
    {
        printf("name: %s, flags: %u, addr: %s\n", x.name.c_str(), x.flag, x.addr->str().c_str());
    }
}