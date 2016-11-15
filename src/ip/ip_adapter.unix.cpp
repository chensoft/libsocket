/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/base/basic_socket.hpp>
#include <socket/ip/ip_adapter.hpp>
#include <chen/base/num.hpp>
#include <chen/base/str.hpp>
#include <functional>
#include <algorithm>
#include <ifaddrs.h>
#include <net/if.h>
#include <cstdlib>
#include <cstring>
#include <memory>

#ifdef __linux__
#define AF_LINK AF_PACKET
#else
#include <net/if_dl.h>
#endif

// -----------------------------------------------------------------------------
// helper
namespace
{
    void visit(std::function<void (struct ::ifaddrs *ptr, bool &stop)> cb)
    {
        struct ::ifaddrs *list = nullptr;
        if (::getifaddrs(&list) != 0)
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

            ::freeifaddrs(list);
        }
        catch (...)
        {
            ::freeifaddrs(list);
            throw;
        }
    }

    std::unique_ptr<chen::ip_address> create(struct ::sockaddr *ptr)
    {
        using chen::ip_address;
        using chen::ip_version4;
        using chen::ip_version6;

        if (!ptr)
            return nullptr;

        switch (ptr->sa_family)
        {
            case AF_INET:
                return std::unique_ptr<ip_address>(new ip_address(ip_version4(chen::num::swap(((struct ::sockaddr_in*)ptr)->sin_addr.s_addr))));

            case AF_INET6:
            {
                auto tmp = (struct ::sockaddr_in6*)ptr;
                auto ret = std::unique_ptr<ip_address>(new ip_address(ip_version6(tmp->sin6_addr.s6_addr)));
                ret->scope(tmp->sin6_scope_id);
                return ret;
            }

            default:
                return nullptr;
        }
    }

    void hardware(struct ::ifaddrs *ptr, std::string &mac, std::uint32_t &mtu)
    {
        if (ptr->ifa_addr->sa_family != AF_LINK)
            return;

        // mtu
        chen::basic_socket tmp(AF_INET6, SOCK_DGRAM);

        struct ::ifreq ifr{};
        ifr.ifr_addr.sa_family = AF_INET6;
        ::memcpy(ifr.ifr_name, ptr->ifa_name, IFNAMSIZ);

        if (::ioctl(tmp.native(), SIOCGIFMTU, &ifr) >= 0)
            mtu = ifr.ifr_mtu;

        // mac
#ifdef __linux__
        const std::uint8_t *data = nullptr;
        if (::ioctl(tmp.native(), SIOCGIFHWADDR, &ifr) >= 0)
            data = reinterpret_cast<uint8_t*>(ifr.ifr_hwaddr.sa_data);
#else
        struct ::sockaddr_dl *sdl = (struct ::sockaddr_dl*)ptr->ifa_addr;
        const std::uint8_t  *data = reinterpret_cast<const std::uint8_t*>(LLADDR(sdl));
#endif

        if (data && std::any_of(data, data + 6, [] (std::uint8_t ch) { return ch > 0; }))
            mac = chen::str::format("%02x:%02x:%02x:%02x:%02x:%02x", data[0], data[1], data[2], data[3], data[4], data[5]);
    }

    std::uint8_t netmask(struct ::sockaddr *ptr)
    {
        if (!ptr)
            return 0;

        switch (ptr->sa_family)
        {
            case AF_INET:
                return chen::ip_version4::toCIDR(chen::num::swap(((struct ::sockaddr_in*)ptr)->sin_addr.s_addr));

            case AF_INET6:
                return chen::ip_version6::toCIDR(((struct ::sockaddr_in6*)ptr)->sin6_addr.s6_addr);

            default:
                return 0;
        }
    }
}


// -----------------------------------------------------------------------------
// ifaddr

// enumerate
std::map<std::string, chen::ip_adapter> chen::ip_adapter::enumerate()
{
    std::map<std::string, ip_adapter> map;

    ::visit([&] (struct ::ifaddrs *ptr, bool &stop) {
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
std::uint32_t chen::ip_adapter::scope(const std::uint8_t addr[16], const std::string &name)
{
    // if name is integer
    bool digits = std::all_of(name.begin(), name.end(), [] (char ch) -> bool {
        return std::isdigit(ch);
    });

    if (digits)
        return static_cast<std::uint32_t>(std::atoi(name.c_str()));

    // if name is interface name
    std::uint32_t id = 0;

    ::visit([&] (struct ::ifaddrs *ptr, bool &stop) {
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

std::string chen::ip_adapter::scope(std::uint32_t id)
{
    std::string name;

    ::visit([&] (struct ::ifaddrs *ptr, bool &stop) {
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