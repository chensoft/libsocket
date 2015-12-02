/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_codec.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// codec

// fqdn
bool codec::isFqdn(const std::string &name)
{
    auto size = name.size();
    return size ? name[size - 1] == '.' : false;
}

std::string codec::fqdn(const std::string &name)
{
    return codec::isFqdn(name) ? name : name + ".";
}

// todo @@ put these encode code to dns_codec.cpp
//    // check total length
//    // caution: this limit isn't name's length, it's the bytes after encoded
//    // example: www.chensoft.com. will encoded as [3, w, w, w, 8, c, h, e, n, s, o, f, t, 3, c, o, m, 0]
//    // the encoded bytes can't exceed SIZE_LIMIT_NAME
//    if (qname.size() + 1 > SIZE_LIMIT_NAME)
//        throw error_size(str::format("request query name length must be %d octets or less", SIZE_LIMIT_NAME));
//
//    // check each label
//    std::string::size_type count = 0;
//
//    for (std::uint8_t i = 0, len = static_cast<std::uint8_t>(qname.size()); i < len; ++i)
//    {
//        char c = qname[i];
//
//        if (c == '.')
//        {
//            if (!count)
//                throw error_size("request query label is empty");
//
//            count = 0;
//        }
//        else
//        {
//            ++count;
//
//            if (count > 63)
//                throw error_size("request query label length must be 63 or less");
//        }
//    }
//