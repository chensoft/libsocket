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
std::map<chen::dns::RRType, table::rr_build_type> table::_rr_build = {
        {chen::dns::RRType::A, [] () -> rr_pointer { return rr_pointer(new chen::dns::A); }},
        {chen::dns::RRType::NS, [] () -> rr_pointer { return rr_pointer(new chen::dns::NS); }},
        {chen::dns::RRType::MD, [] () -> rr_pointer { return rr_pointer(new chen::dns::MD); }},
        {chen::dns::RRType::MF, [] () -> rr_pointer { return rr_pointer(new chen::dns::MF); }},
        {chen::dns::RRType::CNAME, [] () -> rr_pointer { return rr_pointer(new chen::dns::CNAME); }},
        {chen::dns::RRType::SOA, [] () -> rr_pointer { return rr_pointer(new chen::dns::SOA); }},
        {chen::dns::RRType::MB, [] () -> rr_pointer { return rr_pointer(new chen::dns::MB); }},
        {chen::dns::RRType::MG, [] () -> rr_pointer { return rr_pointer(new chen::dns::MG); }},
        {chen::dns::RRType::MR, [] () -> rr_pointer { return rr_pointer(new chen::dns::MR); }},
        {chen::dns::RRType::RNULL, [] () -> rr_pointer { return rr_pointer(new chen::dns::RNULL); }},
        {chen::dns::RRType::WKS, [] () -> rr_pointer { return rr_pointer(new chen::dns::WKS); }},
        {chen::dns::RRType::PTR, [] () -> rr_pointer { return rr_pointer(new chen::dns::PTR); }},
        {chen::dns::RRType::HINFO, [] () -> rr_pointer { return rr_pointer(new chen::dns::HINFO); }},
        {chen::dns::RRType::MINFO, [] () -> rr_pointer { return rr_pointer(new chen::dns::MINFO); }},
        {chen::dns::RRType::MX, [] () -> rr_pointer { return rr_pointer(new chen::dns::MX); }},
        {chen::dns::RRType::TXT, [] () -> rr_pointer { return rr_pointer(new chen::dns::TXT); }},
        {chen::dns::RRType::RP, [] () -> rr_pointer { return rr_pointer(new chen::dns::RP); }},
        {chen::dns::RRType::AFSDB, [] () -> rr_pointer { return rr_pointer(new chen::dns::AFSDB); }},
        {chen::dns::RRType::X25, [] () -> rr_pointer { return rr_pointer(new chen::dns::X25); }},
        {chen::dns::RRType::ISDN, [] () -> rr_pointer { return rr_pointer(new chen::dns::ISDN); }},
        {chen::dns::RRType::RT, [] () -> rr_pointer { return rr_pointer(new chen::dns::RT); }},
        {chen::dns::RRType::NSAP, [] () -> rr_pointer { return rr_pointer(new chen::dns::NSAP); }},
        {chen::dns::RRType::NSAPPTR, [] () -> rr_pointer { return rr_pointer(new chen::dns::NSAPPTR); }},
        {chen::dns::RRType::SIG, [] () -> rr_pointer { return rr_pointer(new chen::dns::SIG); }},
        {chen::dns::RRType::KEY, [] () -> rr_pointer { return rr_pointer(new chen::dns::KEY); }},
        {chen::dns::RRType::PX, [] () -> rr_pointer { return rr_pointer(new chen::dns::PX); }},
        {chen::dns::RRType::GPOS, [] () -> rr_pointer { return rr_pointer(new chen::dns::GPOS); }},
        {chen::dns::RRType::AAAA, [] () -> rr_pointer { return rr_pointer(new chen::dns::AAAA); }},
        {chen::dns::RRType::LOC, [] () -> rr_pointer { return rr_pointer(new chen::dns::LOC); }},
        {chen::dns::RRType::NXT, [] () -> rr_pointer { return rr_pointer(new chen::dns::NXT); }},
        {chen::dns::RRType::EID, [] () -> rr_pointer { return rr_pointer(new chen::dns::EID); }},
        {chen::dns::RRType::NIMLOC, [] () -> rr_pointer { return rr_pointer(new chen::dns::NIMLOC); }},
        {chen::dns::RRType::SRV, [] () -> rr_pointer { return rr_pointer(new chen::dns::SRV); }},
        {chen::dns::RRType::ATMA, [] () -> rr_pointer { return rr_pointer(new chen::dns::ATMA); }},
        {chen::dns::RRType::NAPTR, [] () -> rr_pointer { return rr_pointer(new chen::dns::NAPTR); }},
        {chen::dns::RRType::KX, [] () -> rr_pointer { return rr_pointer(new chen::dns::KX); }},
        {chen::dns::RRType::CERT, [] () -> rr_pointer { return rr_pointer(new chen::dns::CERT); }},
        {chen::dns::RRType::DNAME, [] () -> rr_pointer { return rr_pointer(new chen::dns::DNAME); }},
        {chen::dns::RRType::OPT, [] () -> rr_pointer { return rr_pointer(new chen::dns::OPT); }},
        {chen::dns::RRType::DS, [] () -> rr_pointer { return rr_pointer(new chen::dns::DS); }},
        {chen::dns::RRType::SSHFP, [] () -> rr_pointer { return rr_pointer(new chen::dns::SSHFP); }},
        {chen::dns::RRType::IPSECKEY, [] () -> rr_pointer { return rr_pointer(new chen::dns::IPSECKEY); }},
        {chen::dns::RRType::RRSIG, [] () -> rr_pointer { return rr_pointer(new chen::dns::RRSIG); }},
        {chen::dns::RRType::NSEC, [] () -> rr_pointer { return rr_pointer(new chen::dns::NSEC); }},
        {chen::dns::RRType::DNSKEY, [] () -> rr_pointer { return rr_pointer(new chen::dns::DNSKEY); }},
        {chen::dns::RRType::DHCID, [] () -> rr_pointer { return rr_pointer(new chen::dns::DHCID); }},
        {chen::dns::RRType::NSEC3, [] () -> rr_pointer { return rr_pointer(new chen::dns::NSEC3); }},
        {chen::dns::RRType::NSEC3PARAM, [] () -> rr_pointer { return rr_pointer(new chen::dns::NSEC3PARAM); }},
        {chen::dns::RRType::TLSA, [] () -> rr_pointer { return rr_pointer(new chen::dns::TLSA); }},
        {chen::dns::RRType::HIP, [] () -> rr_pointer { return rr_pointer(new chen::dns::HIP); }},
        {chen::dns::RRType::NINFO, [] () -> rr_pointer { return rr_pointer(new chen::dns::NINFO); }},
        {chen::dns::RRType::RKEY, [] () -> rr_pointer { return rr_pointer(new chen::dns::RKEY); }},
        {chen::dns::RRType::TALINK, [] () -> rr_pointer { return rr_pointer(new chen::dns::TALINK); }},
        {chen::dns::RRType::CDS, [] () -> rr_pointer { return rr_pointer(new chen::dns::CDS); }},
        {chen::dns::RRType::CDNSKEY, [] () -> rr_pointer { return rr_pointer(new chen::dns::CDNSKEY); }},
        {chen::dns::RRType::OPENPGPKEY, [] () -> rr_pointer { return rr_pointer(new chen::dns::OPENPGPKEY); }},
        {chen::dns::RRType::SPF, [] () -> rr_pointer { return rr_pointer(new chen::dns::SPF); }},
        {chen::dns::RRType::UINFO, [] () -> rr_pointer { return rr_pointer(new chen::dns::UINFO); }},
        {chen::dns::RRType::UID, [] () -> rr_pointer { return rr_pointer(new chen::dns::UID); }},
        {chen::dns::RRType::GID, [] () -> rr_pointer { return rr_pointer(new chen::dns::GID); }},
        {chen::dns::RRType::UNSPEC, [] () -> rr_pointer { return rr_pointer(new chen::dns::UNSPEC); }},
        {chen::dns::RRType::NID, [] () -> rr_pointer { return rr_pointer(new chen::dns::NID); }},
        {chen::dns::RRType::L32, [] () -> rr_pointer { return rr_pointer(new chen::dns::L32); }},
        {chen::dns::RRType::L64, [] () -> rr_pointer { return rr_pointer(new chen::dns::L64); }},
        {chen::dns::RRType::LP, [] () -> rr_pointer { return rr_pointer(new chen::dns::LP); }},
        {chen::dns::RRType::EUI48, [] () -> rr_pointer { return rr_pointer(new chen::dns::EUI48); }},
        {chen::dns::RRType::EUI64, [] () -> rr_pointer { return rr_pointer(new chen::dns::EUI64); }},
        {chen::dns::RRType::TKEY, [] () -> rr_pointer { return rr_pointer(new chen::dns::TKEY); }},
        {chen::dns::RRType::TSIG, [] () -> rr_pointer { return rr_pointer(new chen::dns::TSIG); }},

        {chen::dns::RRType::URI, [] () -> rr_pointer { return rr_pointer(new chen::dns::URI); }},
        {chen::dns::RRType::CAA, [] () -> rr_pointer { return rr_pointer(new chen::dns::CAA); }},
        {chen::dns::RRType::TA, [] () -> rr_pointer { return rr_pointer(new chen::dns::TA); }},
        {chen::dns::RRType::DLV, [] () -> rr_pointer { return rr_pointer(new chen::dns::DLV); }}
};

std::map<chen::dns::RRType, std::string> table::_rr_type_text = {
        {chen::dns::RRType::A, "A"},
        {chen::dns::RRType::NS, "NS"},
        {chen::dns::RRType::MD, "MD"},
        {chen::dns::RRType::MF, "MF"},
        {chen::dns::RRType::CNAME, "CNAME"},
        {chen::dns::RRType::SOA, "SOA"},
        {chen::dns::RRType::MB, "MB"},
        {chen::dns::RRType::MG, "MG"},
        {chen::dns::RRType::MR, "MR"},
        {chen::dns::RRType::RNULL, "NULL"},  // NULL is the official name
        {chen::dns::RRType::WKS, "WKS"},
        {chen::dns::RRType::PTR, "PTR"},
        {chen::dns::RRType::HINFO, "HINFO"},
        {chen::dns::RRType::MINFO, "MINFO"},
        {chen::dns::RRType::MX, "MX"},
        {chen::dns::RRType::TXT, "TXT"},
        {chen::dns::RRType::RP, "RP"},
        {chen::dns::RRType::AFSDB, "AFSDB"},
        {chen::dns::RRType::X25, "X25"},
        {chen::dns::RRType::ISDN, "ISDN"},
        {chen::dns::RRType::RT, "RT"},
        {chen::dns::RRType::NSAP, "NSAP"},
        {chen::dns::RRType::NSAPPTR, "NSAPPTR"},
        {chen::dns::RRType::SIG, "SIG"},
        {chen::dns::RRType::KEY, "KEY"},
        {chen::dns::RRType::PX, "PX"},
        {chen::dns::RRType::GPOS, "GPOS"},
        {chen::dns::RRType::AAAA, "AAAA"},
        {chen::dns::RRType::LOC, "LOC"},
        {chen::dns::RRType::NXT, "NXT"},
        {chen::dns::RRType::EID, "EID"},
        {chen::dns::RRType::NIMLOC, "NIMLOC"},
        {chen::dns::RRType::SRV, "SRV"},
        {chen::dns::RRType::ATMA, "ATMA"},
        {chen::dns::RRType::NAPTR, "NAPTR"},
        {chen::dns::RRType::KX, "KX"},
        {chen::dns::RRType::CERT, "CERT"},
        {chen::dns::RRType::DNAME, "DNAME"},
        {chen::dns::RRType::OPT, "OPT"},
        {chen::dns::RRType::DS, "DS"},
        {chen::dns::RRType::SSHFP, "SSHFP"},
        {chen::dns::RRType::IPSECKEY, "IPSECKEY"},
        {chen::dns::RRType::RRSIG, "RRSIG"},
        {chen::dns::RRType::NSEC, "NSEC"},
        {chen::dns::RRType::DNSKEY, "DNSKEY"},
        {chen::dns::RRType::DHCID, "DHCID"},
        {chen::dns::RRType::NSEC3, "NSEC3"},
        {chen::dns::RRType::NSEC3PARAM, "NSEC3PARAM"},
        {chen::dns::RRType::TLSA, "TLSA"},
        {chen::dns::RRType::HIP, "HIP"},
        {chen::dns::RRType::NINFO, "NINFO"},
        {chen::dns::RRType::RKEY, "RKEY"},
        {chen::dns::RRType::TALINK, "TALINK"},
        {chen::dns::RRType::CDS, "CDS"},
        {chen::dns::RRType::CDNSKEY, "CDNSKEY"},
        {chen::dns::RRType::OPENPGPKEY, "OPENPGPKEY"},
        {chen::dns::RRType::SPF, "SPF"},
        {chen::dns::RRType::UINFO, "UINFO"},
        {chen::dns::RRType::UID, "UID"},
        {chen::dns::RRType::GID, "GID"},
        {chen::dns::RRType::UNSPEC, "UNSPEC"},
        {chen::dns::RRType::NID, "NID"},
        {chen::dns::RRType::L32, "L32"},
        {chen::dns::RRType::L64, "L64"},
        {chen::dns::RRType::LP, "LP"},
        {chen::dns::RRType::EUI48, "EUI48"},
        {chen::dns::RRType::EUI64, "EUI64"},
        {chen::dns::RRType::TKEY, "TKEY"},
        {chen::dns::RRType::TSIG, "TSIG"},

        {chen::dns::RRType::IXFR, "IXFR"},
        {chen::dns::RRType::AXFR, "AXFR"},
        {chen::dns::RRType::MAILB, "MAILB"},
        {chen::dns::RRType::MAILA, "MAILA"},
        {chen::dns::RRType::ANY, "ANY"},

        {chen::dns::RRType::URI, "URI"},
        {chen::dns::RRType::CAA, "CAA"},
        {chen::dns::RRType::TA, "TA"},
        {chen::dns::RRType::DLV, "DLV"}
};

std::map<std::string, chen::dns::RRType> table::_rr_text_type;

std::map<chen::dns::RRClass, std::string> table::_rr_class_text = {
        {chen::dns::RRClass::IN, "IN"},
        {chen::dns::RRClass::CS, "CS"},
        {chen::dns::RRClass::CH, "CH"},
        {chen::dns::RRClass::HS, "HS"},
        {chen::dns::RRClass::NONE, "NONE"},
        {chen::dns::RRClass::ANY, "ANY"}
};

std::map<std::string, chen::dns::RRClass> table::_rr_text_class;

// build
chen::dns::table::rr_pointer table::build(chen::dns::RRType key)
{
    auto it = table::_rr_build.find(key);
    return it != table::_rr_build.cend() ? it->second() : nullptr;
}

// type & text
std::string table::typeToText(chen::dns::RRType key)
{
    auto it = table::_rr_type_text.find(key);
    return it != table::_rr_type_text.cend() ? it->second : "";
}

chen::dns::RRType table::textToType(const std::string &key)
{
    table::init();

    auto it = table::_rr_text_type.find(key);
    return it != table::_rr_text_type.cend() ? it->second : chen::dns::RRType::None;
}

// class & text
std::string table::classToText(chen::dns::RRClass key)
{
    auto it = table::_rr_class_text.find(key);
    return it != table::_rr_class_text.cend() ? it->second : "";
}

chen::dns::RRClass table::textToClass(const std::string &key)
{
    table::init();

    auto it = table::_rr_text_class.find(key);
    return it != table::_rr_text_class.cend() ? it->second : chen::dns::RRClass::IN;
}

// set
void table::set(chen::dns::RRType key, rr_build_type val)
{
    table::_rr_build[key] = val;
}

void table::set(chen::dns::RRType key, const std::string &val)
{
    table::init();

    table::_rr_type_text[key] = val;
    table::_rr_text_type[val] = key;
}

void table::set(chen::dns::RRClass key, const std::string &val)
{
    table::init();

    table::_rr_class_text[key] = val;
    table::_rr_text_class[val] = key;
}

// init
inline void table::init()
{
    if (table::_rr_text_type.empty())
    {
        for (auto &it : table::_rr_type_text)
            table::_rr_text_type[it.second] = it.first;
    }

    if (table::_rr_text_class.empty())
    {
        for (auto &it : table::_rr_class_text)
            table::_rr_text_class[it.second] = it.first;
    }
}