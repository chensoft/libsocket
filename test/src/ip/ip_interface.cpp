/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_interface.hpp>
#include <gtest/gtest.h>

using namespace chen::ip;

TEST(IPInterfaceTest, General)
{
    // todo
    auto list = chen::ip::interface::enumerate();

    for (auto x : list)
    {
        printf("name: %s, flags: %u\n", x.name.c_str(), x.flag);

        if (x.addr)
        {
            if (x.addr->v4())
            {
                auto xxx = x.addr->v4();
                printf("      addr: %s, mask: 0x%x, broad: %s\n", xxx->str().c_str(), xxx->netmask(), xxx->broadcast().str().c_str());
            }
            else if (x.addr->v6())
            {
                auto xxx = x.addr->v6();
                printf("      addr: %s, prefix: %u, scope: 0x%x\n", xxx->str().c_str(), xxx->cidr(), xxx->scope());
            }
        }
    }
}