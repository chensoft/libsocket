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

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// helper
namespace
{
    using rr_pointer = table::rr_pointer;

    std::map<RRType, table::rr_build_type> g_rr_build = {
            {RRType::A, [] () -> rr_pointer { return std::make_shared<A>(); }},
            {RRType::NS, [] () -> rr_pointer { return std::make_shared<NS>(); }},
            {RRType::MD, [] () -> rr_pointer { return std::make_shared<MD>(); }},
            {RRType::MF, [] () -> rr_pointer { return std::make_shared<MF>(); }},
            {RRType::CNAME, [] () -> rr_pointer { return std::make_shared<CNAME>(); }},
            {RRType::SOA, [] () -> rr_pointer { return std::make_shared<SOA>(); }},
            {RRType::MB, [] () -> rr_pointer { return std::make_shared<MB>(); }},
            {RRType::MG, [] () -> rr_pointer { return std::make_shared<MG>(); }},
            {RRType::MR, [] () -> rr_pointer { return std::make_shared<MR>(); }},
            {RRType::Null, [] () -> rr_pointer { return std::make_shared<Null>(); }},
            {RRType::WKS, [] () -> rr_pointer { return std::make_shared<WKS>(); }},
            {RRType::PTR, [] () -> rr_pointer { return std::make_shared<PTR>(); }},
            {RRType::HINFO, [] () -> rr_pointer { return std::make_shared<HINFO>(); }},
            {RRType::MINFO, [] () -> rr_pointer { return std::make_shared<MINFO>(); }},
            {RRType::MX, [] () -> rr_pointer { return std::make_shared<MX>(); }},
            {RRType::TXT, [] () -> rr_pointer { return std::make_shared<TXT>(); }},
            {RRType::RP, [] () -> rr_pointer { return std::make_shared<RP>(); }},
            {RRType::AFSDB, [] () -> rr_pointer { return std::make_shared<AFSDB>(); }},
            {RRType::X25, [] () -> rr_pointer { return std::make_shared<X25>(); }},
            {RRType::ISDN, [] () -> rr_pointer { return std::make_shared<ISDN>(); }},
            {RRType::RT, [] () -> rr_pointer { return std::make_shared<RT>(); }},
            {RRType::NSAP, [] () -> rr_pointer { return std::make_shared<NSAP>(); }},
            {RRType::NSAPPTR, [] () -> rr_pointer { return std::make_shared<NSAPPTR>(); }},
            {RRType::SIG, [] () -> rr_pointer { return std::make_shared<SIG>(); }},
            {RRType::KEY, [] () -> rr_pointer { return std::make_shared<KEY>(); }},
            {RRType::PX, [] () -> rr_pointer { return std::make_shared<PX>(); }},
            {RRType::GPOS, [] () -> rr_pointer { return std::make_shared<GPOS>(); }},
            {RRType::AAAA, [] () -> rr_pointer { return std::make_shared<AAAA>(); }},
            {RRType::LOC, [] () -> rr_pointer { return std::make_shared<LOC>(); }},
            {RRType::NXT, [] () -> rr_pointer { return std::make_shared<NXT>(); }},
            {RRType::EID, [] () -> rr_pointer { return std::make_shared<EID>(); }},
            {RRType::NIMLOC, [] () -> rr_pointer { return std::make_shared<NIMLOC>(); }},
            {RRType::SRV, [] () -> rr_pointer { return std::make_shared<SRV>(); }},
            {RRType::ATMA, [] () -> rr_pointer { return std::make_shared<ATMA>(); }},
            {RRType::NAPTR, [] () -> rr_pointer { return std::make_shared<NAPTR>(); }},
            {RRType::KX, [] () -> rr_pointer { return std::make_shared<KX>(); }},
            {RRType::CERT, [] () -> rr_pointer { return std::make_shared<CERT>(); }},
            {RRType::A6, [] () -> rr_pointer { return std::make_shared<A6>(); }},
            {RRType::DNAME, [] () -> rr_pointer { return std::make_shared<DNAME>(); }},
            {RRType::SINK, [] () -> rr_pointer { return std::make_shared<SINK>(); }},
            {RRType::OPT, [] () -> rr_pointer { return std::make_shared<OPT>(); }},
            {RRType::DS, [] () -> rr_pointer { return std::make_shared<DS>(); }},
            {RRType::SSHFP, [] () -> rr_pointer { return std::make_shared<SSHFP>(); }},
            {RRType::IPSECKEY, [] () -> rr_pointer { return std::make_shared<IPSECKEY>(); }},
            {RRType::RRSIG, [] () -> rr_pointer { return std::make_shared<RRSIG>(); }},
            {RRType::NSEC, [] () -> rr_pointer { return std::make_shared<NSEC>(); }},
            {RRType::DNSKEY, [] () -> rr_pointer { return std::make_shared<DNSKEY>(); }},
            {RRType::DHCID, [] () -> rr_pointer { return std::make_shared<DHCID>(); }},
            {RRType::NSEC3, [] () -> rr_pointer { return std::make_shared<NSEC3>(); }},
            {RRType::NSEC3PARAM, [] () -> rr_pointer { return std::make_shared<NSEC3PARAM>(); }},
            {RRType::TLSA, [] () -> rr_pointer { return std::make_shared<TLSA>(); }},
            {RRType::SMIMEA, [] () -> rr_pointer { return std::make_shared<SMIMEA>(); }},
            {RRType::HIP, [] () -> rr_pointer { return std::make_shared<HIP>(); }},
            {RRType::NINFO, [] () -> rr_pointer { return std::make_shared<NINFO>(); }},
            {RRType::RKEY, [] () -> rr_pointer { return std::make_shared<RKEY>(); }},
            {RRType::TALINK, [] () -> rr_pointer { return std::make_shared<TALINK>(); }},
            {RRType::CDS, [] () -> rr_pointer { return std::make_shared<CDS>(); }},
            {RRType::CDNSKEY, [] () -> rr_pointer { return std::make_shared<CDNSKEY>(); }},
            {RRType::OPENPGPKEY, [] () -> rr_pointer { return std::make_shared<OPENPGPKEY>(); }},
            {RRType::CSYNC, [] () -> rr_pointer { return std::make_shared<CSYNC>(); }},
            {RRType::SPF, [] () -> rr_pointer { return std::make_shared<SPF>(); }},
            {RRType::UINFO, [] () -> rr_pointer { return std::make_shared<UINFO>(); }},
            {RRType::UID, [] () -> rr_pointer { return std::make_shared<UID>(); }},
            {RRType::GID, [] () -> rr_pointer { return std::make_shared<GID>(); }},
            {RRType::UNSPEC, [] () -> rr_pointer { return std::make_shared<UNSPEC>(); }},
            {RRType::NID, [] () -> rr_pointer { return std::make_shared<NID>(); }},
            {RRType::L32, [] () -> rr_pointer { return std::make_shared<L32>(); }},
            {RRType::L64, [] () -> rr_pointer { return std::make_shared<L64>(); }},
            {RRType::LP, [] () -> rr_pointer { return std::make_shared<LP>(); }},
            {RRType::EUI48, [] () -> rr_pointer { return std::make_shared<EUI48>(); }},
            {RRType::EUI64, [] () -> rr_pointer { return std::make_shared<EUI64>(); }},
            {RRType::TKEY, [] () -> rr_pointer { return std::make_shared<TKEY>(); }},
            {RRType::TSIG, [] () -> rr_pointer { return std::make_shared<TSIG>(); }},

            {RRType::URI, [] () -> rr_pointer { return std::make_shared<URI>(); }},
            {RRType::CAA, [] () -> rr_pointer { return std::make_shared<CAA>(); }},
            {RRType::TA, [] () -> rr_pointer { return std::make_shared<TA>(); }},
            {RRType::DLV, [] () -> rr_pointer { return std::make_shared<DLV>(); }}
    };

    using opt_pointer = table::opt_pointer;

    std::map<edns0::OptionCode, table::opt_build_type> g_opt_build {
            {edns0::OptionCode::Subnet, [] () -> opt_pointer { return std::make_shared<edns0::Subnet>(); }}
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

    std::unordered_map<std::string, RRClass> g_rr_text_class;

    std::map<QR, std::string> g_rr_qr_text = {
            {QR::Query, "QUERY"},
            {QR::Response, "RESPONSE"}
    };

    std::unordered_map<std::string, QR> g_rr_text_qr;

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
table::rr_pointer table::build(RRType key)
{
    auto it = g_rr_build.find(key);
    return it != g_rr_build.end() ? it->second() : nullptr;
}

table::opt_pointer table::build(edns0::OptionCode key)
{
    auto it = g_opt_build.find(key);
    return it != g_opt_build.end() ? it->second() : nullptr;
}

// type & text
std::string table::typeToText(RRType key)
{
    auto it = g_rr_type_text.find(key);
    return it != g_rr_type_text.end() ? it->second : "";
}

RRType table::textToType(const std::string &key)
{
    auto it = g_rr_text_type.find(key);
    return it != g_rr_text_type.end() ? it->second : RRType::None;
}

// class & text
std::string table::classToText(RRClass key)
{
    auto it = g_rr_class_text.find(key);
    return it != g_rr_class_text.end() ? it->second : "";
}

RRClass table::textToClass(const std::string &key)
{
    auto it = g_rr_text_class.find(key);
    return it != g_rr_text_class.end() ? it->second : RRClass::IN;
}

// qr & text
std::string table::qrToText(QR key)
{
    auto it = g_rr_qr_text.find(key);
    return it != g_rr_qr_text.end() ? it->second : "";
}

QR table::textToQr(const std::string &key)
{
    auto it = g_rr_text_qr.find(key);
    return it != g_rr_text_qr.end() ? it->second : QR::Query;
}

// opcode & text
std::string table::opcodeToText(OPCODE key)
{
    auto it = g_rr_opcode_text.find(key);
    return it != g_rr_opcode_text.end() ? it->second : "";
}

OPCODE table::textToOpcode(const std::string &key)
{
    auto it = g_rr_text_opcode.find(key);
    return it != g_rr_text_opcode.end() ? it->second : OPCODE::Query;
}

// rcode & text
std::string table::rcodeToText(RCODE key)
{
    auto it = g_rr_rcode_text.find(key);
    return it != g_rr_rcode_text.end() ? it->second : "";
}

RCODE table::textToRcode(const std::string &key)
{
    auto it = g_rr_text_rcode.find(key);
    return it != g_rr_text_rcode.end() ? it->second : RCODE::NoError;
}

// set
void table::set(RRType key, rr_build_type val)
{
    g_rr_build[key] = val;
}

void table::set(edns0::OptionCode key, opt_build_type val)
{
    g_opt_build[key] = val;
}

void table::set(RRType key, const std::string &val)
{
    g_rr_type_text[key] = val;
    g_rr_text_type[val] = key;
}

void table::set(RRClass key, const std::string &val)
{
    g_rr_class_text[key] = val;
    g_rr_text_class[val] = key;
}

void table::set(QR key, const std::string &val)
{
    g_rr_qr_text[key] = val;
    g_rr_text_qr[val] = key;
}

void table::set(OPCODE key, const std::string &val)
{
    g_rr_opcode_text[key] = val;
    g_rr_text_opcode[val] = key;
}

void table::set(RCODE key, const std::string &val)
{
    g_rr_rcode_text[key] = val;
    g_rr_text_rcode[val] = key;
}