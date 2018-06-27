/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include "socket/inet/inet_adapter.hpp"
#include "chen/base/str.hpp"
#include "chen/base/num.hpp"
#include <Iphlpapi.h>
#include <functional>
#include <codecvt>
#include <cstring>
#include <cctype>
#include <memory>

#pragma comment(lib, "Iphlpapi.lib")

// -----------------------------------------------------------------------------
// helper
namespace
{
    std::string utf8(wchar_t *wide)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        return utf8_conv.to_bytes(wide);
    }

    void visit(std::function<void (PIP_ADAPTER_ADDRESSES ptr, bool &stop)> cb)
    {
        ULONG size = 0;
        if ((::GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &size) != ERROR_BUFFER_OVERFLOW) || !size)
            return;

        auto list = (PIP_ADAPTER_ADDRESSES)::malloc(size);

        if (::GetAdaptersAddresses(AF_UNSPEC, 0, NULL, list, &size) != ERROR_SUCCESS)
        {
            ::free(list);
            return;
        }

        try
        {
            bool stop = false;

            for (auto ptr = list; ptr != nullptr; ptr = ptr->Next)
            {
                cb(ptr, stop);
                if (stop)
                    break;
            }

            ::free(list);
        }
        catch (...)
        {
            ::free(list);
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
                return std::unique_ptr<ip_address>(new ip_address(ip_version4(chen::num::swap(((::sockaddr_in*)ptr)->sin_addr.s_addr))));

            case AF_INET6:
            {
                auto tmp = (::sockaddr_in6*)ptr;
                auto ret = std::unique_ptr<ip_address>(new ip_address(ip_version6(tmp->sin6_addr.s6_addr)));
                ret->scope(tmp->sin6_scope_id);
                return ret;
            }

            default:
                return nullptr;
        }
    }

    void hardware(PIP_ADAPTER_ADDRESSES ptr, std::string &mac, std::uint32_t &mtu)
    {
        // mtu
        mtu = ptr->Mtu;

        // mac
        for (ULONG i = 0; i < ptr->PhysicalAddressLength; ++i)
        {
            if (i > 0)
            mac += ':';

            mac += chen::str::format("%02x", ptr->PhysicalAddress[i]);
        }
    }
}


// -----------------------------------------------------------------------------
// ifaddr

// enumerate
std::map<std::string, chen::inet_adapter> chen::inet_adapter::enumerate()
{
    std::map<std::string, inet_adapter> map;

    ::visit([&](PIP_ADAPTER_ADDRESSES ptr, bool &stop) {
        auto  name = ::utf8(ptr->FriendlyName);
        auto &item = map[name];

        // name and flag
        if (item.name.empty())
        {
            item.name = name;
            item.flag = ptr->Flags;
        }

        // mac and mtu
        ::hardware(ptr, item.mac, item.mtu);

        // address and netmask
        for (auto *pAddr = ptr->FirstUnicastAddress; pAddr != nullptr; pAddr = pAddr->Next)
        {
            auto addr = ::create(pAddr->Address.lpSockaddr);

            if (addr)
            {
                if (pAddr->OnLinkPrefixLength != 255)
                    addr->cidr(pAddr->OnLinkPrefixLength);

                item.addr.emplace_back(*addr);
            }
        }
    });

    return map;
}

// scope
std::uint32_t chen::inet_adapter::scope(const std::uint8_t addr[16], const std::string &name)
{
    // scope is always a number on Windows
    return static_cast<std::uint32_t>(std::atoi(name.c_str()));
}

std::string chen::inet_adapter::scope(std::uint32_t id)
{
    // scope is always a number on Windows
    return num::str(id);
}

#endif