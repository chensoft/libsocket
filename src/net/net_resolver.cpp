/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstdlib>
#include <cctype>

// -----------------------------------------------------------------------------
// resolver
std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &mixed)
{
    return resolver::resolve(mixed, ip::address::Type::None);
}

std::vector<chen::net::endpoint> chen::net::resolver::resolve(const std::string &mixed, ip::address::Type type)
{
    auto split = resolver::extract(mixed);
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
    {
        net::endpoint ret;
        ret.addr(ip::address(ip::version4(0u)));
        ret.port(port);
        return {ret};
    }

    struct ::addrinfo *info = nullptr;
    struct ::addrinfo  hint{};

    hint.ai_family   = static_cast<int>(type);
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(host.c_str(), nullptr, &hint, &info))
        return {};

    std::vector<net::endpoint> ret;

    try
    {
        for (struct ::addrinfo *ptr = info; ptr != nullptr; ptr = ptr->ai_next)
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
std::pair<std::string, std::uint16_t> chen::net::resolver::extract(const std::string &mixed)
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

    return std::make_pair(std::move(first), resolver::service(second));
}