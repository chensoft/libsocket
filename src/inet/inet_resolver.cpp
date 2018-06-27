/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "socket/inet/inet_resolver.hpp"
#include "chen/base/num.hpp"
#include <cstdlib>
#include <cstring>
#include <cctype>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

// -----------------------------------------------------------------------------
// resolver
std::vector<chen::inet_address> chen::inet_resolver::resolve(const std::string &mixed, int family)
{
    auto split = inet_resolver::extract(mixed);

    if (split.first.empty())
        return {inet_address(ip_version4(0u), inet_resolver::service(split.second))};

    ::addrinfo *info = nullptr;
    ::addrinfo  hint{};

    hint.ai_family   = family;
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(split.first.c_str(), !split.second.empty() ? split.second.c_str() : nullptr, &hint, &info))
        return {};

    std::vector<inet_address> ret;

    try
    {
        for (auto ptr = info; ptr != nullptr; ptr = ptr->ai_next)
            ret.emplace_back(inet_address(ptr->ai_addr));

        ::freeaddrinfo(info);
    }
    catch (...)
    {
        ::freeaddrinfo(info);
        throw;
    }

    return ret;
}

// reverse
std::pair<std::string, std::string> chen::inet_resolver::reverse(const inet_address &addr)
{
    char host[NI_MAXHOST]{};
    char serv[NI_MAXSERV]{};

    auto tmp = addr.sockaddr();
    auto ret = ::getnameinfo((const ::sockaddr*)tmp.get(), addr.socklen(), host, NI_MAXHOST, serv, NI_MAXSERV, 0);

    return !ret ? std::make_pair(std::string(host), std::string(serv)) : std::pair<std::string, std::string>();
}

// service
std::uint16_t chen::inet_resolver::service(const std::string &name, const std::string &protocol)
{
    if (name.empty())
        return 0;

    if (std::isdigit(name[0]))
        return static_cast<std::uint16_t>(std::atoi(name.c_str()));

    if (protocol.empty())
    {
        // search tcp first, then udp
        auto ent = ::getservbyname(name.c_str(), "tcp");
        if (ent)
            return num::swap(static_cast<std::uint16_t>(ent->s_port));

        ent = ::getservbyname(name.c_str(), "udp");
        return ent ? num::swap(static_cast<std::uint16_t>(ent->s_port)) : static_cast<std::uint16_t>(0);
    }
    else
    {
        // search specific protocol
        auto ent = ::getservbyname(name.c_str(), protocol.c_str());
        return ent ? num::swap(static_cast<std::uint16_t>(ent->s_port)) : static_cast<std::uint16_t>(0);
    }
}

std::string chen::inet_resolver::service(std::uint16_t port, const std::string &protocol)
{
    auto ent = ::getservbyport(num::swap(port), !protocol.empty() ? protocol.c_str() : nullptr);
    return ent ? ent->s_name : "";
}

// split
std::pair<std::string, std::string> chen::inet_resolver::extract(const std::string &mixed)
{
    auto beg = mixed.data();
    auto len = mixed.size();
    auto end = beg + len;
    auto ptr = beg;

    std::pair<std::string, std::string> ret;

    if (*ptr == '[')
    {
        // IPv6:Port
        auto sep = mixed.rfind(']');
        if (sep == std::string::npos)
            throw std::runtime_error("resolver: IPv6 address format error");

        ret.first.resize(sep - 1);
        ::memcpy((void*)ret.first.data(), beg + 1, sep - 1);

        ptr += sep + 1;
    }
    else
    {
        // IPv4:Port or Domain:Port
        auto sep = (std::min)(len, mixed.rfind(':', len - 1));

        if (sep)
        {
            // Or just IPv6 address(more than two colons)
            if ((sep < len) && (mixed.rfind(':', sep - 1) != std::string::npos))
                sep = len;

            ret.first.resize(sep);
            ::memcpy((void*)ret.first.data(), beg, sep);

            ptr += sep;
        }
    }

    // port
    if ((ptr != end) && (*ptr == ':'))
    {
        auto off = static_cast<std::size_t>(end - ++ptr);  // >= 0

        ret.second.resize(off);
        ::memcpy((void*)ret.second.data(), ptr, off);
    }

    return ret;
}