/**
 * Created by Jian Chen
 * @since  2016.01.26
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/sys.hpp>
#include <sstream>
#include <random>

// -----------------------------------------------------------------------------
// sys
std::error_code chen::sys::error()
{
    return std::error_code(errno, std::system_category());
}

std::string chen::sys::uuid()
{
    std::string unique("xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx");

    std::stringstream ss;
    std::random_device rd;

    for (std::size_t i = 0, len = unique.size(); i < len; ++i)
    {
        auto c = unique[i];
        if ((c != 'x') && (c != 'y'))
            continue;

        auto r = (int)(((float)rd() / std::random_device::max()) * 16) | 0;
        auto v = c == 'x' ? r : ((r & 0x3) | 0x8);

        ss.str("");
        ss << std::hex << std::uppercase << v;

        unique[i] = ss.str()[0];
    }

    return unique;
}