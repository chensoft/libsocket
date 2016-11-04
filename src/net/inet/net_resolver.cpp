/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/inet/net_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstdlib>
#include <cctype>

// -----------------------------------------------------------------------------
// resolver
std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &mixed, ip::address::Type type)
{
    auto split = resolver::extract(mixed);
    return resolver::resolve(split.first, split.second, type);
}

std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &host, std::uint16_t port, ip::address::Type type)
{
    auto ret = resolver::resolve(host, "", type);

    for (auto &ep : ret)
        ep.port(port);

    return ret;
}

std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &host, const std::string &service, ip::address::Type type)
{
    if (host.empty())
        return {net::endpoint(ip::version4(0u), resolver::service(service))};

    struct ::addrinfo *info = nullptr;
    struct ::addrinfo  hint{};

    hint.ai_family   = static_cast<int>(type);
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(host.c_str(), !service.empty() ? service.c_str() : nullptr, &hint, &info))
        return {};

    std::vector<net::endpoint> ret;

    try
    {
        for (auto ptr = info; ptr != nullptr; ptr = ptr->ai_next)
            ret.emplace_back(net::endpoint(ptr->ai_addr));

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
std::pair<std::string, std::string> chen::net::resolver::reverse(const net::endpoint &ep)
{
    char host[NI_MAXHOST]{};
    char serv[NI_MAXSERV]{};

    chen::basic_endpoint tmp = static_cast<chen::basic_endpoint>(ep);

    if (!::getnameinfo((const struct sockaddr*)&tmp.addr, tmp.size, host, NI_MAXHOST, serv, NI_MAXSERV, 0))
        return std::make_pair(host, serv);
    else
        return {};
}

// service
std::uint16_t chen::net::resolver::service(const std::string &name, const std::string &protocol)
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

std::string chen::net::resolver::service(std::uint16_t port, const std::string &protocol)
{
    auto ent = ::getservbyport(num::swap(port), !protocol.empty() ? protocol.c_str() : nullptr);
    return ent ? ent->s_name : "";
}

// split
std::pair<std::string, std::string> chen::net::resolver::extract(const std::string &mixed)
{
    if (mixed.empty())
        return {};

    auto cur = mixed.begin();
    auto end = mixed.end();

    std::string first;
    std::string second;

    if (*cur == '[')
    {
        // IPv6:Port
        while ((++cur != end) && (*cur != ']'))
            first += *cur;

        if ((cur == end) || (*cur != ']'))
            throw std::runtime_error("resolver: IPv6 endpoint format error");
        else
            ++cur;
    }
    else
    {
        // IPv4:Port or Domain:Port
        while ((cur != end) && (*cur != ':'))
            first += *cur++;
    }

    // port
    if ((cur != end) && (*cur == ':'))
    {
        while (++cur != end)
            second += *cur;
    }

    return std::make_pair(std::move(first), std::move(second));
}