/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstdlib>
#include <cstring>
#include <cctype>

// -----------------------------------------------------------------------------
// resolver
std::vector<chen::inet_endpoint> chen::inet_resolver::resolve(const std::string &mixed, ip_address::Type type)
{
    auto split = inet_resolver::extract(mixed);
    return inet_resolver::resolve(split.first, split.second, type);
}

std::vector<chen::inet_endpoint> chen::inet_resolver::resolve(const std::string &host, std::uint16_t port, ip_address::Type type)
{
    auto ret = inet_resolver::resolve(host, "", type);

    for (auto &ep : ret)
        ep.port(port);

    return ret;
}

std::vector<chen::inet_endpoint> chen::inet_resolver::resolve(const std::string &host, const std::string &service, ip_address::Type type)
{
    if (host.empty())
        return {inet_endpoint(ip_version4(0u), inet_resolver::service(service))};

    struct ::addrinfo *info = nullptr;
    struct ::addrinfo  hint{};

    hint.ai_family   = static_cast<int>(type);
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(host.c_str(), !service.empty() ? service.c_str() : nullptr, &hint, &info))
        return {};

    std::vector<inet_endpoint> ret;

    try
    {
        for (auto ptr = info; ptr != nullptr; ptr = ptr->ai_next)
            ret.emplace_back(inet_endpoint(ptr->ai_addr));

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
std::pair<std::string, std::string> chen::inet_resolver::reverse(const inet_endpoint &ep)
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
    if (mixed.empty())
        return {};

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
            throw std::runtime_error("resolver: IPv6 endpoint format error");

        ret.first.resize(sep - 1);
        ::memcpy((void*)ret.first.data(), beg + 1, sep - 1);

        ptr += sep + 1;
    }
    else
    {
        // IPv4:Port or Domain:Port
        auto sep = std::min(len, mixed.rfind(':'));

        if (sep)
        {
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