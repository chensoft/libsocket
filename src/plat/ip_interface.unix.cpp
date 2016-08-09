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
#include <netdb.h>

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// helper
namespace
{
    std::shared_ptr<address> create_address(struct sockaddr *ptr)
    {
        if (!ptr)
            return nullptr;

        switch (ptr->sa_family)
        {
            case AF_INET:
                return std::make_shared<address_v4>(num::swap(((struct sockaddr_in*)ptr)->sin_addr.s_addr));

            case AF_INET6:
            {
                auto tmp = (struct sockaddr_in6*)ptr;
                auto ret = std::make_shared<address_v6>(tmp->sin6_addr.s6_addr);
                ret->scope(tmp->sin6_scope_id);
                return ret;
            }

            default:
                return nullptr;
        }
    }

    std::uint8_t create_cidr(struct sockaddr *ptr)
    {
        if (!ptr)
            return 0;

        switch (ptr->sa_family)
        {
            case AF_INET:
                return address_v4::toCIDR(num::swap(((struct sockaddr_in*)ptr)->sin_addr.s_addr));

            case AF_INET6:
                return address_v6::toCIDR(((struct sockaddr_in6*)ptr)->sin6_addr.s6_addr);

            default:
                return 0;
        }
    }
}


// -----------------------------------------------------------------------------
// interface

// flags
bool interface::isUp() const
{
    return (this->flag & IFF_UP) != 0;
}

bool interface::isBroadcast() const
{
    return (this->flag & IFF_BROADCAST) != 0;
}

bool interface::isLoopback() const
{
    return (this->flag & IFF_LOOPBACK) != 0;
}

bool interface::isRunning() const
{
    return (this->flag & IFF_RUNNING) != 0;
}

bool interface::isPromiscuous() const
{
    return (this->flag & IFF_PROMISC) != 0;
}

bool interface::isMulticast() const
{
    return (this->flag & IFF_MULTICAST) != 0;
}

// enumerate
std::vector<interface> interface::enumerate()
{
    struct ifaddrs *list = nullptr;

    if (::getifaddrs(&list) < 0)
        throw error_interface(str::format("if: enumerate error: %s", chen::sys::error().c_str()));

    std::vector<interface> ret;

    try
    {
        for (auto ptr = list; ptr != nullptr; ptr = ptr->ifa_next)
        {
            interface item;
            item.name = ptr->ifa_name;
            item.flag = ptr->ifa_flags;
            item.addr = create_address(ptr->ifa_addr);

            if (item.addr)
            {
                if (ptr->ifa_netmask)
                    item.addr->cidr(create_cidr(ptr->ifa_netmask));
            }
            else
            {
                continue;
            }

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