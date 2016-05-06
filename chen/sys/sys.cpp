/**
 * Created by Jian Chen
 * @since  2016.01.26
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "sys.hpp"
#include <chen/base/date.hpp>
#include <sstream>
#include <random>

using namespace chen;

// -----------------------------------------------------------------------------
// sys
std::string sys::uuid()
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