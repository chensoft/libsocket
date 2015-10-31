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


// -----------------------------------------------------------------------------
// pack

// name
std::size_t pack::nameToBinary(const std::string &name, std::vector<std::uint8_t> &store)
{
    // Note:
    // assume name is valid
    // each label is split by dot
    // label count + label value(exclude dot)
    std::size_t origin = store.size();
    std::size_t count  = 0;

    store.push_back(0);  // size for next label

    for (std::uint8_t i = 0, len = static_cast<std::uint8_t>(name.size()); i < len; ++i)
    {
        char c = name[i];

        if (c == '.')
        {
            store[store.size() - count - 1] = static_cast<std::uint8_t>(count);
            store.push_back(0);  // size for next label

            count = 0;
        }
        else
        {
            ++count;

            store.push_back(static_cast<std::uint8_t>(c));
        }
    }

    return store.size() - origin;
}