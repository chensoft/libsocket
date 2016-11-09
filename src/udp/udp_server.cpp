/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_resolver.hpp>
#include <socket/udp/udp_server.hpp>
#include <chen/base/str.hpp>

// -----------------------------------------------------------------------------
// udp_server
chen::udp_server::udp_server(std::uint16_t port, ip_address::Type type)
{
    this->reset(inet_address(ip_address(type), port));
}

chen::udp_server::udp_server(const char *mixed, ip_address::Type type) : udp_server(std::string(mixed), type)
{
}

chen::udp_server::udp_server(const std::string &mixed, ip_address::Type type)
{
    auto ret = inet_resolver::resolve(mixed, type);
    if (ret.empty())
        throw std::runtime_error(str::format("udp: server resolve address '%s' fail", mixed.c_str()));

    this->reset(ret.front());
}

chen::udp_server::udp_server(const std::string &host, std::uint16_t port, ip_address::Type type)
{
    auto ret = inet_resolver::resolve(host, port, type);
    if (ret.empty())
        throw std::runtime_error(str::format("udp: server resolve address '%s' fail", host.c_str()));

    this->reset(ret.front());
}

chen::udp_server::udp_server(const std::string &host, const std::string &service, ip_address::Type type)
{
    auto ret = inet_resolver::resolve(host, service, type);
    if (ret.empty())
        throw std::runtime_error(str::format("udp: server resolve address '%s' or service '%s' fail", host.c_str(), service.c_str()));

    this->reset(ret.front());
}

chen::udp_server::udp_server(const inet_address &addr) : super_class(addr)
{
}