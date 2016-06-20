/**
 * Created by Jian Chen
 * @since  2015.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_table.hpp>
#include <socket/dns/dns_record.hpp>
#include <unordered_map>
#include <functional>
#include <map>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// helper
namespace
{
    using rr_pointer = chen::dns::table::rr_pointer;

    std::map<chen::dns::RRType, table::rr_build_type> g_rr_build = {
            {chen::dns::RRType::A, [] () -> rr_pointer { return std::make_shared<chen::dns::A>(); }},
            {chen::dns::RRType::NS, [] () -> rr_pointer { return std::make_shared<chen::dns::NS>(); }},
            {chen::dns::RRType::MD, [] () -> rr_pointer { return std::make_shared<chen::dns::MD>(); }},
            {chen::dns::RRType::MF, [] () -> rr_pointer { return std::make_shared<chen::dns::MF>(); }},
            {chen::dns::RRType::CNAME, [] () -> rr_pointer { return std::make_shared<chen::dns::CNAME>(); }},
            {chen::dns::RRType::SOA, [] () -> rr_pointer { return std::make_shared<chen::dns::SOA>(); }},
            {chen::dns::RRType::MB, [] () -> rr_pointer { return std::make_shared<chen::dns::MB>(); }},
            {chen::dns::RRType::MG, [] () -> rr_pointer { return std::make_shared<chen::dns::MG>(); }},
            {chen::dns::RRType::MR, [] () -> rr_pointer { return std::make_shared<chen::dns::MR>(); }},
            {chen::dns::RRType::NUL, [] () -> rr_pointer { return std::make_shared<chen::dns::NUL>(); }},
            {chen::dns::RRType::WKS, [] () -> rr_pointer { return std::make_shared<chen::dns::WKS>(); }},
            {chen::dns::RRType::PTR, [] () -> rr_pointer { return std::make_shared<chen::dns::PTR>(); }},
            {chen::dns::RRType::HINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::HINFO>(); }},
            {chen::dns::RRType::MINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::MINFO>(); }},
            {chen::dns::RRType::MX, [] () -> rr_pointer { return std::make_shared<chen::dns::MX>(); }},
            {chen::dns::RRType::TXT, [] () -> rr_pointer { return std::make_shared<chen::dns::TXT>(); }},
            {chen::dns::RRType::RP, [] () -> rr_pointer { return std::make_shared<chen::dns::RP>(); }},
            {chen::dns::RRType::AFSDB, [] () -> rr_pointer { return std::make_shared<chen::dns::AFSDB>(); }},
            {chen::dns::RRType::X25, [] () -> rr_pointer { return std::make_shared<chen::dns::X25>(); }},
            {chen::dns::RRType::ISDN, [] () -> rr_pointer { return std::make_shared<chen::dns::ISDN>(); }},
            {chen::dns::RRType::RT, [] () -> rr_pointer { return std::make_shared<chen::dns::RT>(); }},
            {chen::dns::RRType::NSAP, [] () -> rr_pointer { return std::make_shared<chen::dns::NSAP>(); }},
            {chen::dns::RRType::NSAPPTR, [] () -> rr_pointer { return std::make_shared<chen::dns::NSAPPTR>(); }},
            {chen::dns::RRType::SIG, [] () -> rr_pointer { return std::make_shared<chen::dns::SIG>(); }},
            {chen::dns::RRType::KEY, [] () -> rr_pointer { return std::make_shared<chen::dns::KEY>(); }},
            {chen::dns::RRType::PX, [] () -> rr_pointer { return std::make_shared<chen::dns::PX>(); }},
            {chen::dns::RRType::GPOS, [] () -> rr_pointer { return std::make_shared<chen::dns::GPOS>(); }},
            {chen::dns::RRType::AAAA, [] () -> rr_pointer { return std::make_shared<chen::dns::AAAA>(); }},
            {chen::dns::RRType::LOC, [] () -> rr_pointer { return std::make_shared<chen::dns::LOC>(); }},
            {chen::dns::RRType::NXT, [] () -> rr_pointer { return std::make_shared<chen::dns::NXT>(); }},
            {chen::dns::RRType::EID, [] () -> rr_pointer { return std::make_shared<chen::dns::EID>(); }},
            {chen::dns::RRType::NIMLOC, [] () -> rr_pointer { return std::make_shared<chen::dns::NIMLOC>(); }},
            {chen::dns::RRType::SRV, [] () -> rr_pointer { return std::make_shared<chen::dns::SRV>(); }},
            {chen::dns::RRType::ATMA, [] () -> rr_pointer { return std::make_shared<chen::dns::ATMA>(); }},
            {chen::dns::RRType::NAPTR, [] () -> rr_pointer { return std::make_shared<chen::dns::NAPTR>(); }},
            {chen::dns::RRType::KX, [] () -> rr_pointer { return std::make_shared<chen::dns::KX>(); }},
            {chen::dns::RRType::CERT, [] () -> rr_pointer { return std::make_shared<chen::dns::CERT>(); }},
            {chen::dns::RRType::A6, [] () -> rr_pointer { return std::make_shared<chen::dns::A6>(); }},
            {chen::dns::RRType::DNAME, [] () -> rr_pointer { return std::make_shared<chen::dns::DNAME>(); }},
            {chen::dns::RRType::SINK, [] () -> rr_pointer { return std::make_shared<chen::dns::SINK>(); }},
            {chen::dns::RRType::OPT, [] () -> rr_pointer { return std::make_shared<chen::dns::OPT>(); }},
            {chen::dns::RRType::DS, [] () -> rr_pointer { return std::make_shared<chen::dns::DS>(); }},
            {chen::dns::RRType::SSHFP, [] () -> rr_pointer { return std::make_shared<chen::dns::SSHFP>(); }},
            {chen::dns::RRType::IPSECKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::IPSECKEY>(); }},
            {chen::dns::RRType::RRSIG, [] () -> rr_pointer { return std::make_shared<chen::dns::RRSIG>(); }},
            {chen::dns::RRType::NSEC, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC>(); }},
            {chen::dns::RRType::DNSKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::DNSKEY>(); }},
            {chen::dns::RRType::DHCID, [] () -> rr_pointer { return std::make_shared<chen::dns::DHCID>(); }},
            {chen::dns::RRType::NSEC3, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC3>(); }},
            {chen::dns::RRType::NSEC3PARAM, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC3PARAM>(); }},
            {chen::dns::RRType::TLSA, [] () -> rr_pointer { return std::make_shared<chen::dns::TLSA>(); }},
            {chen::dns::RRType::SMIMEA, [] () -> rr_pointer { return std::make_shared<chen::dns::SMIMEA>(); }},
            {chen::dns::RRType::HIP, [] () -> rr_pointer { return std::make_shared<chen::dns::HIP>(); }},
            {chen::dns::RRType::NINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::NINFO>(); }},
            {chen::dns::RRType::RKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::RKEY>(); }},
            {chen::dns::RRType::TALINK, [] () -> rr_pointer { return std::make_shared<chen::dns::TALINK>(); }},
            {chen::dns::RRType::CDS, [] () -> rr_pointer { return std::make_shared<chen::dns::CDS>(); }},
            {chen::dns::RRType::CDNSKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::CDNSKEY>(); }},
            {chen::dns::RRType::OPENPGPKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::OPENPGPKEY>(); }},
            {chen::dns::RRType::CSYNC, [] () -> rr_pointer { return std::make_shared<chen::dns::CSYNC>(); }},
            {chen::dns::RRType::SPF, [] () -> rr_pointer { return std::make_shared<chen::dns::SPF>(); }},
            {chen::dns::RRType::UINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::UINFO>(); }},
            {chen::dns::RRType::UID, [] () -> rr_pointer { return std::make_shared<chen::dns::UID>(); }},
            {chen::dns::RRType::GID, [] () -> rr_pointer { return std::make_shared<chen::dns::GID>(); }},
            {chen::dns::RRType::UNSPEC, [] () -> rr_pointer { return std::make_shared<chen::dns::UNSPEC>(); }},
            {chen::dns::RRType::NID, [] () -> rr_pointer { return std::make_shared<chen::dns::NID>(); }},
            {chen::dns::RRType::L32, [] () -> rr_pointer { return std::make_shared<chen::dns::L32>(); }},
            {chen::dns::RRType::L64, [] () -> rr_pointer { return std::make_shared<chen::dns::L64>(); }},
            {chen::dns::RRType::LP, [] () -> rr_pointer { return std::make_shared<chen::dns::LP>(); }},
            {chen::dns::RRType::EUI48, [] () -> rr_pointer { return std::make_shared<chen::dns::EUI48>(); }},
            {chen::dns::RRType::EUI64, [] () -> rr_pointer { return std::make_shared<chen::dns::EUI64>(); }},
            {chen::dns::RRType::TKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::TKEY>(); }},
            {chen::dns::RRType::TSIG, [] () -> rr_pointer { return std::make_shared<chen::dns::TSIG>(); }},

            {chen::dns::RRType::URI, [] () -> rr_pointer { return std::make_shared<chen::dns::URI>(); }},
            {chen::dns::RRType::CAA, [] () -> rr_pointer { return std::make_shared<chen::dns::CAA>(); }},
            {chen::dns::RRType::TA, [] () -> rr_pointer { return std::make_shared<chen::dns::TA>(); }},
            {chen::dns::RRType::DLV, [] () -> rr_pointer { return std::make_shared<chen::dns::DLV>(); }}
    };

    std::map<chen::dns::RRType, std::string> g_rr_type_text = {
            {chen::dns::RRType::A, "A"},
            {chen::dns::RRType::NS, "NS"},
            {chen::dns::RRType::MD, "MD"},
            {chen::dns::RRType::MF, "MF"},
            {chen::dns::RRType::CNAME, "CNAME"},
            {chen::dns::RRType::SOA, "SOA"},
            {chen::dns::RRType::MB, "MB"},
            {chen::dns::RRType::MG, "MG"},
            {chen::dns::RRType::MR, "MR"},
            {chen::dns::RRType::NUL, "NULL"},  // NULL is the official name
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
            {chen::dns::RRType::NSAPPTR, "NSAP-PTR"},  // NSAP-PTR is the official name
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
            {chen::dns::RRType::A6, "A6"},
            {chen::dns::RRType::DNAME, "DNAME"},
            {chen::dns::RRType::SINK, "SINK"},
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
            {chen::dns::RRType::SMIMEA, "SMIMEA"},
            {chen::dns::RRType::HIP, "HIP"},
            {chen::dns::RRType::NINFO, "NINFO"},
            {chen::dns::RRType::RKEY, "RKEY"},
            {chen::dns::RRType::TALINK, "TALINK"},
            {chen::dns::RRType::CDS, "CDS"},
            {chen::dns::RRType::CDNSKEY, "CDNSKEY"},
            {chen::dns::RRType::OPENPGPKEY, "OPENPGPKEY"},
            {chen::dns::RRType::CSYNC, "CSYNC"},
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

    std::unordered_map<std::string, chen::dns::RRType> g_rr_text_type;

    std::map<chen::dns::RRClass, std::string> g_rr_class_text = {
            {chen::dns::RRClass::IN, "IN"},
            {chen::dns::RRClass::CS, "CS"},
            {chen::dns::RRClass::CH, "CH"},
            {chen::dns::RRClass::HS, "HS"},
            {chen::dns::RRClass::NONE, "NONE"},
            {chen::dns::RRClass::ANY, "ANY"}
    };

    std::unordered_map<std::string, chen::dns::RRClass> g_rr_text_class;

    std::map<chen::dns::QR, std::string> g_rr_qr_text = {
            {chen::dns::QR::Query, "QUERY"},
            {chen::dns::QR::Response, "RESPONSE"}
    };

    std::unordered_map<std::string, chen::dns::QR> g_rr_text_qr;

    std::map<chen::dns::OPCODE, std::string> g_rr_opcode_text = {
            {chen::dns::OPCODE::Query, "QUERY"},
            {chen::dns::OPCODE::IQuery, "IQUERY"},
            {chen::dns::OPCODE::Status, "STATUS"},
            {chen::dns::OPCODE::Notify, "NOTIFY"},
            {chen::dns::OPCODE::Update, "UPDATE"}
    };

    std::unordered_map<std::string, chen::dns::OPCODE> g_rr_text_opcode;

    std::map<chen::dns::RCODE, std::string> g_rr_rcode_text = {
            {chen::dns::RCODE::NoError, "NOERROR"},
            {chen::dns::RCODE::FormErr, "FORMERR"},
            {chen::dns::RCODE::ServFail, "SERVFAIL"},
            {chen::dns::RCODE::NXDomain, "NXDOMAIN"},
            {chen::dns::RCODE::NotImp, "NOTIMPL"},
            {chen::dns::RCODE::Refused, "REFUSED"},
            {chen::dns::RCODE::YXDomain, "YXDOMAIN"},
            {chen::dns::RCODE::YXRrSet, "YXRRSET"},
            {chen::dns::RCODE::NXRrSet, "NXRRSET"},
            {chen::dns::RCODE::NotAuth, "NOTAUTH"},
            {chen::dns::RCODE::NotZone, "NOTZONE"},
            {chen::dns::RCODE::BadVers, "BADVERS"},  // BadVers & BadSig has the same value
//        {chen::dns::RCODE::BadSig, "BADSIG"},
            {chen::dns::RCODE::BadKey, "BADKEY"},
            {chen::dns::RCODE::BadTime, "BADTIME"},
            {chen::dns::RCODE::BadMode, "BADMODE"},
            {chen::dns::RCODE::BadName, "BADNAME"},
            {chen::dns::RCODE::BadAlg, "BADALG"},
            {chen::dns::RCODE::BadTrunc, "BADTRUNC"},
            {chen::dns::RCODE::BadCookie, "BADCOOKIE"},
    };

    std::unordered_map<std::string, chen::dns::RCODE> g_rr_text_rcode;

    // helper
    class helper
    {
    public:
        helper()
        {
            for (auto &it : g_rr_type_text)
                g_rr_text_type[it.second] = it.first;

            for (auto &it : g_rr_class_text)
                g_rr_text_class[it.second] = it.first;

            for (auto &it : g_rr_qr_text)
                g_rr_text_qr[it.second] = it.first;

            for (auto &it : g_rr_opcode_text)
                g_rr_text_opcode[it.second] = it.first;

            for (auto &it : g_rr_rcode_text)
                g_rr_text_rcode[it.second] = it.first;
        }
    };

    helper init;  // init the table
}


// -----------------------------------------------------------------------------
// table

// build
chen::dns::table::rr_pointer table::build(chen::dns::RRType key)
{
    auto it = g_rr_build.find(key);
    return it != g_rr_build.end() ? it->second() : nullptr;
}

// type & text
std::string table::typeToText(chen::dns::RRType key)
{
    auto it = g_rr_type_text.find(key);
    return it != g_rr_type_text.end() ? it->second : "";
}

chen::dns::RRType table::textToType(const std::string &key)
{
    auto it = g_rr_text_type.find(key);
    return it != g_rr_text_type.end() ? it->second : chen::dns::RRType::None;
}

// class & text
std::string table::classToText(chen::dns::RRClass key)
{
    auto it = g_rr_class_text.find(key);
    return it != g_rr_class_text.end() ? it->second : "";
}

chen::dns::RRClass table::textToClass(const std::string &key)
{
    auto it = g_rr_text_class.find(key);
    return it != g_rr_text_class.end() ? it->second : chen::dns::RRClass::IN;
}

// qr & text
std::string table::qrToText(chen::dns::QR key)
{
    auto it = g_rr_qr_text.find(key);
    return it != g_rr_qr_text.end() ? it->second : "";
}

chen::dns::QR table::textToQr(const std::string &key)
{
    auto it = g_rr_text_qr.find(key);
    return it != g_rr_text_qr.end() ? it->second : chen::dns::QR::Query;
}

// opcode & text
std::string table::opcodeToText(chen::dns::OPCODE key)
{
    auto it = g_rr_opcode_text.find(key);
    return it != g_rr_opcode_text.end() ? it->second : "";
}

chen::dns::OPCODE table::textToOpcode(const std::string &key)
{
    auto it = g_rr_text_opcode.find(key);
    return it != g_rr_text_opcode.end() ? it->second : chen::dns::OPCODE::Query;
}

// rcode & text
std::string table::rcodeToText(chen::dns::RCODE key)
{
    auto it = g_rr_rcode_text.find(key);
    return it != g_rr_rcode_text.end() ? it->second : "";
}

chen::dns::RCODE table::textToRcode(const std::string &key)
{
    auto it = g_rr_text_rcode.find(key);
    return it != g_rr_text_rcode.end() ? it->second : chen::dns::RCODE::NoError;
}

// set
void table::set(chen::dns::RRType key, rr_build_type val)
{
    g_rr_build[key] = val;
}

void table::set(chen::dns::RRType key, const std::string &val)
{
    g_rr_type_text[key] = val;
    g_rr_text_type[val] = key;
}

void table::set(chen::dns::RRClass key, const std::string &val)
{
    g_rr_class_text[key] = val;
    g_rr_text_class[val] = key;
}

void table::set(chen::dns::QR key, const std::string &val)
{
    g_rr_qr_text[key] = val;
    g_rr_text_qr[val] = key;
}

void table::set(chen::dns::OPCODE key, const std::string &val)
{
    g_rr_opcode_text[key] = val;
    g_rr_text_opcode[val] = key;
}

void table::set(chen::dns::RCODE key, const std::string &val)
{
    g_rr_rcode_text[key] = val;
    g_rr_text_rcode[val] = key;
}