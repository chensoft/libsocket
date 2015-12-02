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