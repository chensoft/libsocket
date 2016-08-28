/**
 * Created by Jian Chen
 * @since  2016.08.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstdlib>

// -----------------------------------------------------------------------------
// resolver
std::vector<chen::net::address> chen::net::resolver::address(const std::string &host)
{
    return resolver::address(host, AF_UNSPEC);  // IPv4 or IPv6
}

std::vector<chen::net::address> chen::net::resolver::address(const std::string &host, int family)
{
    struct addrinfo *info = nullptr;
    struct addrinfo hint{};

    hint.ai_family   = family;
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(host.c_str(), nullptr, &hint, &info))
        return {};

    std::vector<chen::net::address> ret;

    for (struct addrinfo *ptr = info; ptr != nullptr; ptr = ptr->ai_next)
        ret.emplace_back(endpoint::toEndpoint(ptr->ai_addr).addr());

    ::freeaddrinfo(info);

    return ret;
}

std::uint16_t chen::net::resolver::service(const std::string &name, const std::string &protocol)
{
    if (name.empty())
        return 0;

    if (std::isdigit(static_cast<std::uint8_t>(name[0])))
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

std::pair<std::string, std::uint16_t> chen::net::resolver::split(const std::string &mixed)
{
    if (mixed.empty())
        return {};

    auto it = mixed.begin();

    std::string first;
    std::string second;

    if (*it == '[')
    {
        // IPv6:Port format
        while (*++it && (*it != ']'))
            first += *it;

        if (*it == ']')
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