/**
 * Created by Jian Chen
 * @since  2015.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_table.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// table
std::map<chen::dns::RRType, std::string> table::_rr_name = {
        {chen::dns::RRType::A, "A"},
        {chen::dns::RRType::NS, "NS"},
        {chen::dns::RRType::CNAME, "CNAME"},
        {chen::dns::RRType::SOA, "SOA"},
        {chen::dns::RRType::PTR, "PTR"},
        {chen::dns::RRType::MX, "MX"},
        {chen::dns::RRType::TXT, "TXT"},
};

std::map<std::string, chen::dns::RRType> table::_rr_reverse = {
        {"A", chen::dns::RRType::A},
        {"NS", chen::dns::RRType::NS},
        {"CNAME", chen::dns::RRType::CNAME},
        {"SOA", chen::dns::RRType::SOA},
        {"PTR", chen::dns::RRType::PTR},
        {"MX", chen::dns::RRType::MX},
        {"TXT", chen::dns::RRType::TXT},
};

std::map<chen::dns::RRType, table::rr_build_type> table::_rr_build = {
        {chen::dns::RRType::A, [] () -> rr_pointer { return rr_pointer(new chen::dns::A); }},
        {chen::dns::RRType::NS, [] () -> rr_pointer { return rr_pointer(new chen::dns::NS); }},
        {chen::dns::RRType::CNAME, [] () -> rr_pointer { return rr_pointer(new chen::dns::CNAME); }},
        {chen::dns::RRType::SOA, [] () -> rr_pointer { return rr_pointer(new chen::dns::SOA); }},
        {chen::dns::RRType::PTR, [] () -> rr_pointer { return rr_pointer(new chen::dns::PTR); }},
        {chen::dns::RRType::MX, [] () -> rr_pointer { return rr_pointer(new chen::dns::MX); }},
        {chen::dns::RRType::TXT, [] () -> rr_pointer { return rr_pointer(new chen::dns::TXT); }},
};

// get
std::string table::rrTypeToName(chen::dns::RRType key)
{
    auto it = table::_rr_name.find(key);
    return it != table::_rr_name.end() ? it->second : "";
}

chen::dns::RRType table::rrNameToType(const std::string &key)
{
    auto it = table::_rr_reverse.find(key);
    return it != table::_rr_reverse.end() ? it->second : chen::dns::RRType::None;
}

chen::dns::table::rr_build_type table::rrTypeToBuild(chen::dns::RRType key)
{
    auto it = table::_rr_build.find(key);
    return it != table::_rr_build.end() ? it->second : nullptr;
}

// set
void table::set(chen::dns::RRType key, const std::string &val)
{
    table::_rr_name[key] = val;
    table::_rr_reverse[val] = key;
}

void table::set(chen::dns::RRType key, rr_build_type val)
{
    table::_rr_build[key] = val;
}