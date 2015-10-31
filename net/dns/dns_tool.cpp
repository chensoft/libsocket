/**
 * Created by Jian Chen
 * @since  2015.10.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_tool.h"
#include "dns_error.h"
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
    // label length + label value(exclude dot)
    std::size_t origin = store.size();
    std::size_t length = 0;

    store.push_back(0);  // size for next label

    for (std::uint8_t i = 0, len = static_cast<std::uint8_t>(name.size()); i < len; ++i)
    {
        char c = name[i];

        if (c == '.')
        {
            store[store.size() - length - 1] = static_cast<std::uint8_t>(length);
            store.push_back(0);  // size for next label

            length = 0;
        }
        else
        {
            ++length;

            store.push_back(static_cast<std::uint8_t>(c));
        }
    }

    return store.size() - origin;
}

std::string pack::binaryToName(const std::uint8_t *data, std::size_t size)
{
    if (!size)
        return "";

    std::string ret;
    const std::uint8_t *ptr = data;

    while (*ptr)
    {
        if (*ptr + 1 > size)
            throw error_size("pack binary to name size is not enough");

        for (std::size_t i = 0; i < *ptr; ++i)
        {
            ret += *(ptr + i + 1);
        }

        ptr += *ptr + 1;
        ret += ".";
    }

    return ret;
}