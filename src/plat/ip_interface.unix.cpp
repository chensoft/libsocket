/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/ip/ip_interface.hpp>
#include <socket/ip/ip_error.hpp>
#include <chen/chen.hpp>
#include <ifaddrs.h>
#include <net/if.h>

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// interface
std::vector<interface> interface::enumerate()
{
    std::vector<interface> ret;
    struct ifaddrs *list = nullptr;

    if (::getifaddrs(&list) < 0)
        throw error_interface(str::format("if: enumerate error: %s", chen::sys::error().c_str()));

    try
    {
        // todo
        for (auto ptr = list; ptr != nullptr; ptr = ptr->ifa_next)
        {
            interface item;
            item.name = ptr->ifa_name;
            item.flag = ptr->ifa_flags;
            item.addr = std::make_shared<address_v4>();

            ret.emplace_back(std::move(item));
        }
    }
    catch (...)
    {
        ::freeifaddrs(list);
        throw;
    }

    ::freeifaddrs(list);
    return ret;
}

#endif