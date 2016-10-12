/**
 * Created by Jian Chen
 * @since  2015.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_table.hpp>
#include <unordered_map>
#include <functional>
#include <map>

// -----------------------------------------------------------------------------
// helper
namespace
{
    using namespace chen::dns;
    using namespace chen::dns::edns0;

    using rr_pointer = chen::dns::table::rr_pointer;
    using opt_pointer = chen::dns::table::opt_pointer;

    std::map<RRType, chen::dns::table::rr_build_type> g_rr_build = {
            {RRType::A, [] () -> rr_pointer { return std::make_shared<chen::dns::A>(); }},
            {RRType::NS, [] () -> rr_pointer { return std::make_shared<chen::dns::NS>(); }},
            {RRType::MD, [] () -> rr_pointer { return std::make_shared<chen::dns::MD>(); }},
            {RRType::MF, [] () -> rr_pointer { return std::make_shared<chen::dns::MF>(); }},
            {RRType::CNAME, [] () -> rr_pointer { return std::make_shared<chen::dns::CNAME>(); }},
            {RRType::SOA, [] () -> rr_pointer { return std::make_shared<chen::dns::SOA>(); }},
            {RRType::MB, [] () -> rr_pointer { return std::make_shared<chen::dns::MB>(); }},
            {RRType::MG, [] () -> rr_pointer { return std::make_shared<chen::dns::MG>(); }},
            {RRType::MR, [] () -> rr_pointer { return std::make_shared<chen::dns::MR>(); }},
            {RRType::Null, [] () -> rr_pointer { return std::make_shared<chen::dns::Null>(); }},
            {RRType::WKS, [] () -> rr_pointer { return std::make_shared<chen::dns::WKS>(); }},
            {RRType::PTR, [] () -> rr_pointer { return std::make_shared<chen::dns::PTR>(); }},
            {RRType::HINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::HINFO>(); }},
            {RRType::MINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::MINFO>(); }},
            {RRType::MX, [] () -> rr_pointer { return std::make_shared<chen::dns::MX>(); }},
            {RRType::TXT, [] () -> rr_pointer { return std::make_shared<chen::dns::TXT>(); }},
            {RRType::RP, [] () -> rr_pointer { return std::make_shared<chen::dns::RP>(); }},
            {RRType::AFSDB, [] () -> rr_pointer { return std::make_shared<chen::dns::AFSDB>(); }},
            {RRType::X25, [] () -> rr_pointer { return std::make_shared<chen::dns::X25>(); }},
            {RRType::ISDN, [] () -> rr_pointer { return std::make_shared<chen::dns::ISDN>(); }},
            {RRType::RT, [] () -> rr_pointer { return std::make_shared<chen::dns::RT>(); }},
            {RRType::NSAP, [] () -> rr_pointer { return std::make_shared<chen::dns::NSAP>(); }},
            {RRType::NSAPPTR, [] () -> rr_pointer { return std::make_shared<chen::dns::NSAPPTR>(); }},
            {RRType::SIG, [] () -> rr_pointer { return std::make_shared<chen::dns::SIG>(); }},
            {RRType::KEY, [] () -> rr_pointer { return std::make_shared<chen::dns::KEY>(); }},
            {RRType::PX, [] () -> rr_pointer { return std::make_shared<chen::dns::PX>(); }},
            {RRType::GPOS, [] () -> rr_pointer { return std::make_shared<chen::dns::GPOS>(); }},
            {RRType::AAAA, [] () -> rr_pointer { return std::make_shared<chen::dns::AAAA>(); }},
            {RRType::LOC, [] () -> rr_pointer { return std::make_shared<chen::dns::LOC>(); }},
            {RRType::NXT, [] () -> rr_pointer { return std::make_shared<chen::dns::NXT>(); }},
            {RRType::EID, [] () -> rr_pointer { return std::make_shared<chen::dns::EID>(); }},
            {RRType::NIMLOC, [] () -> rr_pointer { return std::make_shared<chen::dns::NIMLOC>(); }},
            {RRType::SRV, [] () -> rr_pointer { return std::make_shared<chen::dns::SRV>(); }},
            {RRType::ATMA, [] () -> rr_pointer { return std::make_shared<chen::dns::ATMA>(); }},
            {RRType::NAPTR, [] () -> rr_pointer { return std::make_shared<chen::dns::NAPTR>(); }},
            {RRType::KX, [] () -> rr_pointer { return std::make_shared<chen::dns::KX>(); }},
            {RRType::CERT, [] () -> rr_pointer { return std::make_shared<chen::dns::CERT>(); }},
            {RRType::A6, [] () -> rr_pointer { return std::make_shared<chen::dns::A6>(); }},
            {RRType::DNAME, [] () -> rr_pointer { return std::make_shared<chen::dns::DNAME>(); }},
            {RRType::SINK, [] () -> rr_pointer { return std::make_shared<chen::dns::SINK>(); }},
            {RRType::OPT, [] () -> rr_pointer { return std::make_shared<chen::dns::OPT>(); }},
            {RRType::DS, [] () -> rr_pointer { return std::make_shared<chen::dns::DS>(); }},
            {RRType::SSHFP, [] () -> rr_pointer { return std::make_shared<chen::dns::SSHFP>(); }},
            {RRType::IPSECKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::IPSECKEY>(); }},
            {RRType::RRSIG, [] () -> rr_pointer { return std::make_shared<chen::dns::RRSIG>(); }},
            {RRType::NSEC, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC>(); }},
            {RRType::DNSKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::DNSKEY>(); }},
            {RRType::DHCID, [] () -> rr_pointer { return std::make_shared<chen::dns::DHCID>(); }},
            {RRType::NSEC3, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC3>(); }},
            {RRType::NSEC3PARAM, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC3PARAM>(); }},
            {RRType::TLSA, [] () -> rr_pointer { return std::make_shared<chen::dns::TLSA>(); }},
            {RRType::SMIMEA, [] () -> rr_pointer { return std::make_shared<chen::dns::SMIMEA>(); }},
            {RRType::HIP, [] () -> rr_pointer { return std::make_shared<chen::dns::HIP>(); }},
            {RRType::NINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::NINFO>(); }},
            {RRType::RKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::RKEY>(); }},
            {RRType::TALINK, [] () -> rr_pointer { return std::make_shared<chen::dns::TALINK>(); }},
            {RRType::CDS, [] () -> rr_pointer { return std::make_shared<chen::dns::CDS>(); }},
            {RRType::CDNSKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::CDNSKEY>(); }},
            {RRType::OPENPGPKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::OPENPGPKEY>(); }},
            {RRType::CSYNC, [] () -> rr_pointer { return std::make_shared<chen::dns::CSYNC>(); }},
            {RRType::SPF, [] () -> rr_pointer { return std::make_shared<chen::dns::SPF>(); }},
            {RRType::UINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::UINFO>(); }},
            {RRType::UID, [] () -> rr_pointer { return std::make_shared<chen::dns::UID>(); }},
            {RRType::GID, [] () -> rr_pointer { return std::make_shared<chen::dns::GID>(); }},
            {RRType::UNSPEC, [] () -> rr_pointer { return std::make_shared<chen::dns::UNSPEC>(); }},
            {RRType::NID, [] () -> rr_pointer { return std::make_shared<chen::dns::NID>(); }},
            {RRType::L32, [] () -> rr_pointer { return std::make_shared<chen::dns::L32>(); }},
            {RRType::L64, [] () -> rr_pointer { return std::make_shared<chen::dns::L64>(); }},
            {RRType::LP, [] () -> rr_pointer { return std::make_shared<chen::dns::LP>(); }},
            {RRType::EUI48, [] () -> rr_pointer { return std::make_shared<chen::dns::EUI48>(); }},
            {RRType::EUI64, [] () -> rr_pointer { return std::make_shared<chen::dns::EUI64>(); }},
            {RRType::TKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::TKEY>(); }},
            {RRType::TSIG, [] () -> rr_pointer { return std::make_shared<chen::dns::TSIG>(); }},

            {RRType::URI, [] () -> rr_pointer { return std::make_shared<chen::dns::URI>(); }},
            {RRType::CAA, [] () -> rr_pointer { return std::make_shared<chen::dns::CAA>(); }},
            {RRType::TA, [] () -> rr_pointer { return std::make_shared<chen::dns::TA>(); }},
            {RRType::DLV, [] () -> rr_pointer { return std::make_shared<chen::dns::DLV>(); }}
    };

    std::map<OptionCode, chen::dns::table::opt_build_type> g_opt_build {
            {OptionCode::LLQ, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::LLQ>(); }},
            {OptionCode::UL, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::UL>(); }},
            {OptionCode::NSID, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::NSID>(); }},
            {OptionCode::DAU, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::DAU>(); }},
            {OptionCode::DHU, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::DHU>(); }},
            {OptionCode::N3U, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::N3U>(); }},
            {OptionCode::Subnet, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::Subnet>(); }},
            {OptionCode::EXPIRE, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::EXPIRE>(); }},
            {OptionCode::COOKIE, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::COOKIE>(); }},
            {OptionCode::Keepalive, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::Keepalive>(); }},
            {OptionCode::Padding, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::Padding>(); }},
            {OptionCode::CHAIN, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::CHAIN>(); }}
    };

    std::map<RRType, std::string> g_rr_type_text = {
            {RRType::A, "A"},
            {RRType::NS, "NS"},
            {RRType::MD, "MD"},
            {RRType::MF, "MF"},
            {RRType::CNAME, "CNAME"},
            {RRType::SOA, "SOA"},
            {RRType::MB, "MB"},
            {RRType::MG, "MG"},
            {RRType::MR, "MR"},
            {RRType::Null, "NULL"},  // NULL is the official name
            {RRType::WKS, "WKS"},
            {RRType::PTR, "PTR"},
            {RRType::HINFO, "HINFO"},
            {RRType::MINFO, "MINFO"},
            {RRType::MX, "MX"},
            {RRType::TXT, "TXT"},
            {RRType::RP, "RP"},
            {RRType::AFSDB, "AFSDB"},
            {RRType::X25, "X25"},
            {RRType::ISDN, "ISDN"},
            {RRType::RT, "RT"},
            {RRType::NSAP, "NSAP"},
            {RRType::NSAPPTR, "NSAP-PTR"},  // NSAP-PTR is the official name
            {RRType::SIG, "SIG"},
            {RRType::KEY, "KEY"},
            {RRType::PX, "PX"},
            {RRType::GPOS, "GPOS"},
            {RRType::AAAA, "AAAA"},
            {RRType::LOC, "LOC"},
            {RRType::NXT, "NXT"},
            {RRType::EID, "EID"},
            {RRType::NIMLOC, "NIMLOC"},
            {RRType::SRV, "SRV"},
            {RRType::ATMA, "ATMA"},
            {RRType::NAPTR, "NAPTR"},
            {RRType::KX, "KX"},
            {RRType::CERT, "CERT"},
            {RRType::A6, "A6"},
            {RRType::DNAME, "DNAME"},
            {RRType::SINK, "SINK"},
            {RRType::OPT, "OPT"},
            {RRType::DS, "DS"},
            {RRType::SSHFP, "SSHFP"},
            {RRType::IPSECKEY, "IPSECKEY"},
            {RRType::RRSIG, "RRSIG"},
            {RRType::NSEC, "NSEC"},
            {RRType::DNSKEY, "DNSKEY"},
            {RRType::DHCID, "DHCID"},
            {RRType::NSEC3, "NSEC3"},
            {RRType::NSEC3PARAM, "NSEC3PARAM"},
            {RRType::TLSA, "TLSA"},
            {RRType::SMIMEA, "SMIMEA"},
            {RRType::HIP, "HIP"},
            {RRType::NINFO, "NINFO"},
            {RRType::RKEY, "RKEY"},
            {RRType::TALINK, "TALINK"},
            {RRType::CDS, "CDS"},
            {RRType::CDNSKEY, "CDNSKEY"},
            {RRType::OPENPGPKEY, "OPENPGPKEY"},
            {RRType::CSYNC, "CSYNC"},
            {RRType::SPF, "SPF"},
            {RRType::UINFO, "UINFO"},
            {RRType::UID, "UID"},
            {RRType::GID, "GID"},
            {RRType::UNSPEC, "UNSPEC"},
            {RRType::NID, "NID"},
            {RRType::L32, "L32"},
            {RRType::L64, "L64"},
            {RRType::LP, "LP"},
            {RRType::EUI48, "EUI48"},
            {RRType::EUI64, "EUI64"},
            {RRType::TKEY, "TKEY"},
            {RRType::TSIG, "TSIG"},

            {RRType::IXFR, "IXFR"},
            {RRType::AXFR, "AXFR"},
            {RRType::MAILB, "MAILB"},
            {RRType::MAILA, "MAILA"},
            {RRType::ANY, "ANY"},

            {RRType::URI, "URI"},
            {RRType::CAA, "CAA"},
            {RRType::TA, "TA"},
            {RRType::DLV, "DLV"}
    };

    std::unordered_map<std::string, RRType> g_rr_text_type;

    std::map<RRClass, std::string> g_rr_class_text = {
            {RRClass::IN, "IN"},
            {RRClass::CS, "CS"},
            {RRClass::CH, "CH"},
            {RRClass::HS, "HS"},
            {RRClass::NONE, "NONE"},
            {RRClass::ANY, "ANY"}
    };

    std::unordered_map<std::string, chen::dns::RRClass> g_rr_text_class;

    std::map<QrCode, std::string> g_rr_qr_text = {
            {QrQuery, "QUERY"},
            {QrResponse, "RESPONSE"}
    };

    std::unordered_map<std::string, QrCode> g_rr_text_qr;

    std::map<OPCODE, std::string> g_rr_opcode_text = {
            {OPCODE::Query, "QUERY"},
            {OPCODE::IQuery, "IQUERY"},
            {OPCODE::Status, "STATUS"},
            {OPCODE::Notify, "NOTIFY"},
            {OPCODE::Update, "UPDATE"}
    };

    std::unordered_map<std::string, OPCODE> g_rr_text_opcode;

    std::map<RCODE, std::string> g_rr_rcode_text = {
            {RCODE::NoError, "NOERROR"},
            {RCODE::FormErr, "FORMERR"},
            {RCODE::ServFail, "SERVFAIL"},
            {RCODE::NXDomain, "NXDOMAIN"},
            {RCODE::NotImp, "NOTIMPL"},
            {RCODE::Refused, "REFUSED"},
            {RCODE::YXDomain, "YXDOMAIN"},
            {RCODE::YXRrSet, "YXRRSET"},
            {RCODE::NXRrSet, "NXRRSET"},
            {RCODE::NotAuth, "NOTAUTH"},
            {RCODE::NotZone, "NOTZONE"},
            {RCODE::BadVers, "BADVERS"},  // BadVers & BadSig has the same value
//        {RCODE::BadSig, "BADSIG"},
            {RCODE::BadKey, "BADKEY"},
            {RCODE::BadTime, "BADTIME"},
            {RCODE::BadMode, "BADMODE"},
            {RCODE::BadName, "BADNAME"},
            {RCODE::BadAlg, "BADALG"},
            {RCODE::BadTrunc, "BADTRUNC"},
            {RCODE::BadCookie, "BADCOOKIE"},
    };

    std::unordered_map<std::string, RCODE> g_rr_text_rcode;

    std::map<OptionCode, std::string> g_rr_edns0_text = {
            {OptionCode::LLQ, "LLQ"},
            {OptionCode::UL, "UL"},
            {OptionCode::NSID, "NSID"},
            {OptionCode::DAU, "DAU"},
            {OptionCode::DHU, "DHU"},
            {OptionCode::N3U, "N3U"},
            {OptionCode::Subnet, "Subnet"},
            {OptionCode::EXPIRE, "EXPIRE"},
            {OptionCode::COOKIE, "COOKIE"},
            {OptionCode::Keepalive, "Keepalive"},
            {OptionCode::Padding, "Padding"},
            {OptionCode::CHAIN, "CHAIN"},
    };

    std::unordered_map<std::string, OptionCode> g_rr_text_edns0;

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

            for (auto &it : g_rr_edns0_text)
                g_rr_text_edns0[it.second] = it.first;
        }
    };

    helper init;  // init the table
}


// -----------------------------------------------------------------------------
// table

// build
chen::dns::table::rr_pointer chen::dns::table::build(RRType key)
{
    auto it = g_rr_build.find(key);
    return it != g_rr_build.end() ? it->second() : nullptr;
}

chen::dns::table::opt_pointer chen::dns::table::build(edns0::OptionCode key)
{
    auto it = g_opt_build.find(key);
    return it != g_opt_build.end() ? it->second() : nullptr;
}

// type & text
std::string chen::dns::table::typeToText(RRType key)
{
    auto it = g_rr_type_text.find(key);
    return it != g_rr_type_text.end() ? it->second : "";
}

RRType chen::dns::table::textToType(const std::string &key)
{
    auto it = g_rr_text_type.find(key);
    return it != g_rr_text_type.end() ? it->second : RRType::None;
}

// class & text
std::string chen::dns::table::classToText(RRClass key)
{
    auto it = g_rr_class_text.find(key);
    return it != g_rr_class_text.end() ? it->second : "";
}

RRClass chen::dns::table::textToClass(const std::string &key)
{
    auto it = g_rr_text_class.find(key);
    return it != g_rr_text_class.end() ? it->second : RRClass::IN;
}

// qr & text
std::string chen::dns::table::qrToText(QrCode key)
{
    auto it = g_rr_qr_text.find(key);
    return it != g_rr_qr_text.end() ? it->second : "";
}

QrCode chen::dns::table::textToQr(const std::string &key)
{
    auto it = g_rr_text_qr.find(key);
    return it != g_rr_text_qr.end() ? it->second : QrQuery;
}

// opcode & text
std::string chen::dns::table::opcodeToText(OPCODE key)
{
    auto it = g_rr_opcode_text.find(key);
    return it != g_rr_opcode_text.end() ? it->second : "";
}

OPCODE chen::dns::table::textToOpcode(const std::string &key)
{
    auto it = g_rr_text_opcode.find(key);
    return it != g_rr_text_opcode.end() ? it->second : OPCODE::Query;
}

// rcode & text
std::string chen::dns::table::rcodeToText(RCODE key)
{
    auto it = g_rr_rcode_text.find(key);
    return it != g_rr_rcode_text.end() ? it->second : "";
}

RCODE chen::dns::table::textToRcode(const std::string &key)
{
    auto it = g_rr_text_rcode.find(key);
    return it != g_rr_text_rcode.end() ? it->second : RCODE::NoError;
}

// edns0 option code
std::string chen::dns::table::edns0ToText(edns0::OptionCode key)
{
    auto it = g_rr_edns0_text.find(key);
    return it != g_rr_edns0_text.end() ? it->second : "";
}

chen::dns::edns0::OptionCode chen::dns::table::textToEDNS0(const std::string &key)
{
    auto it = g_rr_text_edns0.find(key);
    return it != g_rr_text_edns0.end() ? it->second : edns0::OptionCode::None;
}

// set
void chen::dns::table::set(RRType key, rr_build_type val)
{
    g_rr_build[key] = val;
}

void chen::dns::table::set(edns0::OptionCode key, opt_build_type val)
{
    g_opt_build[key] = val;
}

void chen::dns::table::set(RRType key, const std::string &val)
{
    g_rr_type_text[key] = val;
    g_rr_text_type[val] = key;
}

void chen::dns::table::set(RRClass key, const std::string &val)
{
    g_rr_class_text[key] = val;
    g_rr_text_class[val] = key;
}

void chen::dns::table::set(QrCode key, const std::string &val)
{
    g_rr_qr_text[key] = val;
    g_rr_text_qr[val] = key;
}

void chen::dns::table::set(OPCODE key, const std::string &val)
{
    g_rr_opcode_text[key] = val;
    g_rr_text_opcode[val] = key;
}

void chen::dns::table::set(RCODE key, const std::string &val)
{
    g_rr_rcode_text[key] = val;
    g_rr_text_rcode[val] = key;
}

void chen::dns::table::set(edns0::OptionCode key, const std::string &val)
{
    g_rr_edns0_text[key] = val;
    g_rr_text_edns0[val] = key;
}