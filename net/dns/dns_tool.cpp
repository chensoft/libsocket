/**
 * Created by Jian Chen
 * @since  2015.10.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_tool.h"
#include <chen/tool/str.h>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// tool

// print
std::string tool::format(const std::vector<std::uint8_t> &vec)
{
    std::string ret("{");

    auto cur = vec.cbegin();
    auto end = vec.cend();

    while (cur != end)
    {
        ret.append(str::format("%02x", *cur));

        if (cur != end - 1)
            ret.append(" ");

        ++cur;
    }

    ret.append("}");

    return ret;
}

// fqdn
bool tool::isFqdn(const std::string &name)
{
    auto size = name.size();
    return size ? name[size - 1] == '.' : false;
}

std::string tool::fqdn(const std::string &name)
{
    return tool::isFqdn(name) ? name : name + ".";
}