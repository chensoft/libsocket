/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstdlib>

// -----------------------------------------------------------------------------
// resolver
std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &mixed)
{
    return resolver::resolve(mixed, ip::address::Type::None);
}

std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &mixed, ip::address::Type type)
{
    auto split = resolver::split(mixed);
    return resolver::resolve(split.first, split.second, type);
}

std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &host, const std::string &service)
{
    return resolver::resolve(host, service, ip::address::Type::None);
}

std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &host, const std::string &service, ip::address::Type type)
{
    return resolver::resolve(host, resolver::service(service), type);
}

std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &host, std::uint16_t port)
{
    return resolver::resolve(host, port, ip::address::Type::None);
}

std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &host, std::uint16_t port, ip::address::Type type)
{
    if (host.empty())
        return {net::endpoint(ip::address(ip::version4(0u)), port)};

    struct addrinfo *info = nullptr;
    struct addrinfo  hint{};

    hint.ai_family   = static_cast<int>(type);
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(host.c_str(), nullptr, &hint, &info))
        return {};

    std::vector<net::endpoint> ret;

    try
    {
        for (struct addrinfo *ptr = info; ptr != nullptr; ptr = ptr->ai_next)
        {
            net::endpoint ep(ptr->ai_addr);
            ep.port(port);
            ret.emplace_back(std::move(ep));
        }
    }
    catch (...)
    {
        ::freeaddrinfo(info);
        throw;
    }

    ::freeaddrinfo(info);

    return ret;
}

// first
chen::net::endpoint chen::net::resolver::first(const std::string &mixed)
{
    return resolver::first(mixed, ip::address::Type::None);
}

chen::net::endpoint chen::net::resolver::first(const std::string &mixed, ip::address::Type type)
{
    auto split = resolver::split(mixed);
    return resolver::first(split.first, split.second, type);
}

chen::net::endpoint chen::net::resolver::first(const std::string &host, const std::string &service)
{
    return resolver::first(host, service, ip::address::Type::None);
}

chen::net::endpoint chen::net::resolver::first(const std::string &host, const std::string &service, ip::address::Type type)
{
    return resolver::first(host, resolver::service(service), type);
}

chen::net::endpoint chen::net::resolver::first(const std::string &host, std::uint16_t port)
{
    return resolver::first(host, port, ip::address::Type::None);
}

chen::net::endpoint chen::net::resolver::first(const std::string &host, std::uint16_t port, ip::address::Type type)
{
    if (host.empty())
        return net::endpoint(ip::address(ip::version4(0u)), port);

    struct addrinfo *info = nullptr;
    struct addrinfo  hint{};

    hint.ai_family   = static_cast<int>(type);
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(host.c_str(), nullptr, &hint, &info))
        return nullptr;

    try
    {
        net::endpoint ret(info ? info->ai_addr : nullptr);
        ret.port(port);

        ::freeaddrinfo(info);

        return ret;
    }
    catch (...)
    {
        ::freeaddrinfo(info);
        throw;
    }
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
std::pair<std::string, std::uint16_t> chen::net::resolver::split(const std::string &mixed)
{
    if (mixed.empty())
        return {};

    auto it = mixed.begin();

    std::string first;
    std::string second;

    if (*it == '[')
    {
        ++it;

        // IPv6:Port format
        while (*it && (*it != ']'))
            first += *it++;

        if (*it != ']')
            throw std::runtime_error("resolver: IPv6 endpoint format error");
        else
            ++it;
    }
    else
    {
        // IPv4:Port or Domain:Port
        while (*it && (*it != ':'))
            first += *it++;
    }

    // port
    if (*it == ':')
    {
        while (*++it)
            second += *it;
    }

    return std::make_pair(std::move(first), resolver::service(second));
}