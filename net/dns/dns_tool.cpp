/**
 * Created by Jian Chen
 * @since  2015.10.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_tool.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// tool

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