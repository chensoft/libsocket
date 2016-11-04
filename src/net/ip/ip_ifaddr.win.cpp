/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/net/ip/ip_ifaddr.hpp>

// -----------------------------------------------------------------------------
// ifaddr

// flags
bool chen::ip::ifaddr::isUp() const
{
    // todo
    return false;
}

bool chen::ip::ifaddr::isBroadcast() const
{
    // todo
    return false;
}

bool chen::ip::ifaddr::isLoopback() const
{
    // todo
    return false;
}

bool chen::ip::ifaddr::isRunning() const
{
    // todo
    return false;
}

bool chen::ip::ifaddr::isPromiscuous() const
{
    // todo
    return false;
}

bool chen::ip::ifaddr::isMulticast() const
{
    // todo
    return false;
}

// enumerate
std::map<std::string, chen::ip::ifaddr> chen::ip::ifaddr::enumerate()
{
    // todo
    return {};
}

// scope
std::uint32_t chen::ip::ifaddr::scope(const std::uint8_t addr[16], const std::string &name)
{
    // todo
    return 0;
}

std::string chen::ip::ifaddr::scope(std::uint32_t id)
{
    // todo
    return "";
}

#endif