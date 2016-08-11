/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/ip/ip_interface.hpp>
#include <chen/base/num.hpp>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <cstdlib>

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// helper
namespace
{
    void visit(std::function<void (struct ifaddrs *ptr, bool &stop)> cb)
    {
        struct ifaddrs *list = nullptr;
        if (::getifaddrs(&list) < 0)
            return;

        try
        {
            bool stop = false;

            for (auto ptr = list; ptr != nullptr; ptr = ptr->ifa_next)
            {
                cb(ptr, stop);
                if (stop)
                    break;
            }
        }
        catch (...)
        {
            ::freeifaddrs(list);
            throw;
        }

        ::freeifaddrs(list);
    }

    std::unique_ptr<address> create(struct sockaddr *ptr)
    {
        if (!ptr)
            return nullptr;

        switch (ptr->sa_family)
        {
            case AF_INET:
                return std::unique_ptr<address>(new address(num::swap(((struct sockaddr_in*)ptr)->sin_addr.s_addr)));

            case AF_INET6:
            {
                auto tmp = (struct sockaddr_in6*)ptr;
                auto ret = std::unique_ptr<address>(new address(tmp->sin6_addr.s6_addr));
                ret->scope(tmp->sin6_scope_id);
                return ret;
            }

            default:
                return nullptr;
        }
    }

    std::uint8_t netmask(struct sockaddr *ptr)
    {
        if (!ptr)
            return 0;

        switch (ptr->sa_family)
        {
            case AF_INET:
                return version4::toCIDR(num::swap(((struct sockaddr_in*)ptr)->sin_addr.s_addr));

            case AF_INET6:
                return version6::toCIDR(((struct sockaddr_in6*)ptr)->sin6_addr.s6_addr);

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
std::map<std::string, interface> interface::enumerate()
{
    std::map<std::string, interface> map;

    visit([&] (struct ifaddrs *ptr, bool &stop) {
        auto &item = map[ptr->ifa_name];

        if (item.name.empty())
        {
            item.name = ptr->ifa_name;
            item.flag = ptr->ifa_flags;
        }

        auto addr = create(ptr->ifa_addr);

        if (addr)
        {
            if (ptr->ifa_netmask)
                addr->cidr(netmask(ptr->ifa_netmask));

            item.addr.emplace_back(*addr);
        }
    });

    return map;
}

// scope
std::uint32_t interface::scope(const std::array<std::uint8_t, 16> &addr, const std::string &name)
{
    // if name is integer
    bool digits = std::all_of(name.begin(), name.end(), [] (char ch) -> bool {
        return std::isdigit(ch);
    });

    if (digits)
        return static_cast<std::uint32_t>(std::atoi(name.c_str()));

    // if name is interface name
    std::uint32_t id = 0;

    visit([&] (struct ifaddrs *ptr, bool &stop) {
        if ((name != ptr->ifa_name) || !ptr->ifa_addr || (ptr->ifa_addr->sa_family != AF_INET6))
            return;

        // check address
        auto tmp = (struct sockaddr_in6*)ptr->ifa_addr;

        if (std::equal(addr.begin(), addr.end(), tmp->sin6_addr.s6_addr))
        {
            id   = tmp->sin6_scope_id;
            stop = true;
        }
    });

    return id;
}

std::string interface::scope(std::uint32_t id)
{
    std::string name;

    visit([&] (struct ifaddrs *ptr, bool &stop) {
        if (!ptr->ifa_addr || (ptr->ifa_addr->sa_family != AF_INET6))
            return;

        auto tmp = (struct sockaddr_in6*)ptr->ifa_addr;

        if (tmp->sin6_scope_id == id)
        {
            name = ptr->ifa_name;
            stop = true;
        }
    });

    return !name.empty() ? name : num::str(id);
}

#endif