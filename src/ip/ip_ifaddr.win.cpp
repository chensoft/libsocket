/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/ip/ip_ifaddr.hpp>

// -----------------------------------------------------------------------------
// ifaddr

// flags
bool chen::ip_ifaddr::isUp() const
{
    // todo
    return false;
}

bool chen::ip_ifaddr::isBroadcast() const
{
    // todo
    return false;
}

bool chen::ip_ifaddr::isLoopback() const
{
    // todo
    return false;
}

bool chen::ip_ifaddr::isRunning() const
{
    // todo
    return false;
}

bool chen::ip_ifaddr::isPromiscuous() const
{
    // todo
    return false;
}

bool chen::ip_ifaddr::isMulticast() const
{
    // todo
    return false;
}

// enumerate
std::map<std::string, chen::ip_ifaddr> chen::ip_ifaddr::enumerate()
{
    // todo
    return {};
}

// scope
std::uint32_t chen::ip_ifaddr::scope(const std::uint8_t addr[16], const std::string &name)
{
    // todo
    return 0;
}

std::string chen::ip_ifaddr::scope(std::uint32_t id)
{
    // todo
    return "";
}

#endif