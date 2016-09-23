/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/ip/ip_interface.hpp>
#include <chen/base/num.hpp>
#include <chen/base/str.hpp>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <ifaddrs.h>
#include <net/if.h>

#ifdef __APPLE__
#include <net/if_dl.h>
#elif !defined(AF_LINK)
#define AF_LINK AF_PACKET
#endif

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

    std::unique_ptr<chen::ip::address> create(struct ::sockaddr *ptr)
    {
        using chen::ip::address;

        if (!ptr)
            return nullptr;

        switch (ptr->sa_family)
        {
            case AF_INET:
                return std::unique_ptr<address>(new address(chen::num::swap(((struct ::sockaddr_in*)ptr)->sin_addr.s_addr)));

            case AF_INET6:
            {
                auto tmp = (struct ::sockaddr_in6*)ptr;
                auto ret = std::unique_ptr<address>(new address(tmp->sin6_addr.s6_addr));
                ret->scope(tmp->sin6_scope_id);
                return ret;
            }

            default:
                return nullptr;
        }
    }

    void hardware(struct ifaddrs *ptr, std::string &mac, std::int32_t &mtu)
    {
        if (ptr->ifa_addr->sa_family != AF_LINK)
            return;

        // mtu
        int fd = ::socket(AF_INET6, SOCK_DGRAM, 0);
        if (fd < 0)
            return;

        struct ifreq ifr{};
        ifr.ifr_addr.sa_family = AF_INET6;
        ::memcpy(ifr.ifr_name, ptr->ifa_name, IFNAMSIZ);

        if (::ioctl(fd, SIOCGIFMTU, &ifr) >= 0)
            mtu = ifr.ifr_mtu;

        // mac
#ifdef __APPLE__
        struct ::sockaddr_dl *sdl  = (struct ::sockaddr_dl*)ptr->ifa_addr;
        const std::uint8_t *data = reinterpret_cast<const std::uint8_t*>(LLADDR(sdl));
#else
        const std::uint8_t *data = nullptr;
        if (::ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0)
            data = reinterpret_cast<uint8_t*>(ifr.ifr_hwaddr.sa_data);
#endif

        if (data && std::any_of(data, data + 6, [] (std::uint8_t ch) { return ch > 0; }))
            mac = chen::str::format("%02x:%02x:%02x:%02x:%02x:%02x", data[0], data[1], data[2], data[3], data[4], data[5]);

        // clean
        ::close(fd);
    }

    std::uint8_t netmask(struct ::sockaddr *ptr)
    {
        using chen::ip::version4;
        using chen::ip::version6;

        if (!ptr)
            return 0;

        switch (ptr->sa_family)
        {
            case AF_INET:
                return version4::toCIDR(chen::num::swap(((struct ::sockaddr_in*)ptr)->sin_addr.s_addr));

            case AF_INET6:
                return version6::toCIDR(((struct ::sockaddr_in6*)ptr)->sin6_addr.s6_addr);

            default:
                return 0;
        }
    }
}


// -----------------------------------------------------------------------------
// interface

// flags
bool chen::ip::interface::isUp() const
{
    return (this->flag & IFF_UP) != 0;
}

bool chen::ip::interface::isBroadcast() const
{
    return (this->flag & IFF_BROADCAST) != 0;
}

bool chen::ip::interface::isLoopback() const
{
    return (this->flag & IFF_LOOPBACK) != 0;
}

bool chen::ip::interface::isRunning() const
{
    return (this->flag & IFF_RUNNING) != 0;
}

bool chen::ip::interface::isPromiscuous() const
{
    return (this->flag & IFF_PROMISC) != 0;
}

bool chen::ip::interface::isMulticast() const
{
    return (this->flag & IFF_MULTICAST) != 0;
}

// enumerate
std::map<std::string, chen::ip::interface> chen::ip::interface::enumerate()
{
    std::map<std::string, interface> map;

    ::visit([&] (struct ifaddrs *ptr, bool &stop) {
        auto &item = map[ptr->ifa_name];

        // name and flag
        if (item.name.empty())
        {
            item.name = ptr->ifa_name;
            item.flag = ptr->ifa_flags;
        }

        // mac and mtu
        ::hardware(ptr, item.mac, item.mtu);

        // address
        auto addr = ::create(ptr->ifa_addr);

        if (addr)
        {
            // netmask
            if (ptr->ifa_netmask)
                addr->cidr(::netmask(ptr->ifa_netmask));

            item.addr.emplace_back(*addr);
        }
    });

    return map;
}

// scope
std::uint32_t chen::ip::interface::scope(const std::uint8_t addr[16], const std::string &name)
{
    // if name is integer
    bool digits = std::all_of(name.begin(), name.end(), [] (char ch) -> bool {
        return std::isdigit(ch);
    });

    if (digits)
        return static_cast<std::uint32_t>(std::atoi(name.c_str()));

    // if name is interface name
    std::uint32_t id = 0;

    ::visit([&] (struct ifaddrs *ptr, bool &stop) {
        if ((name != ptr->ifa_name) || !ptr->ifa_addr || (ptr->ifa_addr->sa_family != AF_INET6))
            return;

        // check address
        auto tmp = (struct ::sockaddr_in6*)ptr->ifa_addr;

        if (!::memcmp(addr, tmp->sin6_addr.s6_addr, 16))
        {
            id   = tmp->sin6_scope_id;
            stop = true;
        }
    });

    return id;
}

std::string chen::ip::interface::scope(std::uint32_t id)
{
    std::string name;

    ::visit([&] (struct ifaddrs *ptr, bool &stop) {
        if (!ptr->ifa_addr || (ptr->ifa_addr->sa_family != AF_INET6))
            return;

        auto tmp = (struct ::sockaddr_in6*)ptr->ifa_addr;

        if (tmp->sin6_scope_id == id)
        {
            name = ptr->ifa_name;
            stop = true;
        }
    });

    return !name.empty() ? name : num::str(id);
}

#endif