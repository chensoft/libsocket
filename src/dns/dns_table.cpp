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
            {TypeA, [] () -> rr_pointer { return std::make_shared<chen::dns::A>(); }},
            {TypeNS, [] () -> rr_pointer { return std::make_shared<chen::dns::NS>(); }},
            {TypeMD, [] () -> rr_pointer { return std::make_shared<chen::dns::MD>(); }},
            {TypeMF, [] () -> rr_pointer { return std::make_shared<chen::dns::MF>(); }},
            {TypeCNAME, [] () -> rr_pointer { return std::make_shared<chen::dns::CNAME>(); }},
            {TypeSOA, [] () -> rr_pointer { return std::make_shared<chen::dns::SOA>(); }},
            {TypeMB, [] () -> rr_pointer { return std::make_shared<chen::dns::MB>(); }},
            {TypeMG, [] () -> rr_pointer { return std::make_shared<chen::dns::MG>(); }},
            {TypeMR, [] () -> rr_pointer { return std::make_shared<chen::dns::MR>(); }},
            {TypeNULL, [] () -> rr_pointer { return std::make_shared<chen::dns::Null>(); }},
            {TypeWKS, [] () -> rr_pointer { return std::make_shared<chen::dns::WKS>(); }},
            {TypePTR, [] () -> rr_pointer { return std::make_shared<chen::dns::PTR>(); }},
            {TypeHINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::HINFO>(); }},
            {TypeMINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::MINFO>(); }},
            {TypeMX, [] () -> rr_pointer { return std::make_shared<chen::dns::MX>(); }},
            {TypeTXT, [] () -> rr_pointer { return std::make_shared<chen::dns::TXT>(); }},
            {TypeRP, [] () -> rr_pointer { return std::make_shared<chen::dns::RP>(); }},
            {TypeAFSDB, [] () -> rr_pointer { return std::make_shared<chen::dns::AFSDB>(); }},
            {TypeX25, [] () -> rr_pointer { return std::make_shared<chen::dns::X25>(); }},
            {TypeISDN, [] () -> rr_pointer { return std::make_shared<chen::dns::ISDN>(); }},
            {TypeRT, [] () -> rr_pointer { return std::make_shared<chen::dns::RT>(); }},
            {TypeNSAP, [] () -> rr_pointer { return std::make_shared<chen::dns::NSAP>(); }},
            {TypeNSAPPTR, [] () -> rr_pointer { return std::make_shared<chen::dns::NSAPPTR>(); }},
            {TypeSIG, [] () -> rr_pointer { return std::make_shared<chen::dns::SIG>(); }},
            {TypeKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::KEY>(); }},
            {TypePX, [] () -> rr_pointer { return std::make_shared<chen::dns::PX>(); }},
            {TypeGPOS, [] () -> rr_pointer { return std::make_shared<chen::dns::GPOS>(); }},
            {TypeAAAA, [] () -> rr_pointer { return std::make_shared<chen::dns::AAAA>(); }},
            {TypeLOC, [] () -> rr_pointer { return std::make_shared<chen::dns::LOC>(); }},
            {TypeNXT, [] () -> rr_pointer { return std::make_shared<chen::dns::NXT>(); }},
            {TypeEID, [] () -> rr_pointer { return std::make_shared<chen::dns::EID>(); }},
            {TypeNIMLOC, [] () -> rr_pointer { return std::make_shared<chen::dns::NIMLOC>(); }},
            {TypeSRV, [] () -> rr_pointer { return std::make_shared<chen::dns::SRV>(); }},
            {TypeATMA, [] () -> rr_pointer { return std::make_shared<chen::dns::ATMA>(); }},
            {TypeNAPTR, [] () -> rr_pointer { return std::make_shared<chen::dns::NAPTR>(); }},
            {TypeKX, [] () -> rr_pointer { return std::make_shared<chen::dns::KX>(); }},
            {TypeCERT, [] () -> rr_pointer { return std::make_shared<chen::dns::CERT>(); }},
            {TypeA6, [] () -> rr_pointer { return std::make_shared<chen::dns::A6>(); }},
            {TypeDNAME, [] () -> rr_pointer { return std::make_shared<chen::dns::DNAME>(); }},
            {TypeSINK, [] () -> rr_pointer { return std::make_shared<chen::dns::SINK>(); }},
            {TypeOPT, [] () -> rr_pointer { return std::make_shared<chen::dns::OPT>(); }},
            {TypeDS, [] () -> rr_pointer { return std::make_shared<chen::dns::DS>(); }},
            {TypeSSHFP, [] () -> rr_pointer { return std::make_shared<chen::dns::SSHFP>(); }},
            {TypeIPSECKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::IPSECKEY>(); }},
            {TypeRRSIG, [] () -> rr_pointer { return std::make_shared<chen::dns::RRSIG>(); }},
            {TypeNSEC, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC>(); }},
            {TypeDNSKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::DNSKEY>(); }},
            {TypeDHCID, [] () -> rr_pointer { return std::make_shared<chen::dns::DHCID>(); }},
            {TypeNSEC3, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC3>(); }},
            {TypeNSEC3PARAM, [] () -> rr_pointer { return std::make_shared<chen::dns::NSEC3PARAM>(); }},
            {TypeTLSA, [] () -> rr_pointer { return std::make_shared<chen::dns::TLSA>(); }},
            {TypeSMIMEA, [] () -> rr_pointer { return std::make_shared<chen::dns::SMIMEA>(); }},
            {TypeHIP, [] () -> rr_pointer { return std::make_shared<chen::dns::HIP>(); }},
            {TypeNINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::NINFO>(); }},
            {TypeRKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::RKEY>(); }},
            {TypeTALINK, [] () -> rr_pointer { return std::make_shared<chen::dns::TALINK>(); }},
            {TypeCDS, [] () -> rr_pointer { return std::make_shared<chen::dns::CDS>(); }},
            {TypeCDNSKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::CDNSKEY>(); }},
            {TypeOPENPGPKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::OPENPGPKEY>(); }},
            {TypeCSYNC, [] () -> rr_pointer { return std::make_shared<chen::dns::CSYNC>(); }},
            {TypeSPF, [] () -> rr_pointer { return std::make_shared<chen::dns::SPF>(); }},
            {TypeUINFO, [] () -> rr_pointer { return std::make_shared<chen::dns::UINFO>(); }},
            {TypeUID, [] () -> rr_pointer { return std::make_shared<chen::dns::UID>(); }},
            {TypeGID, [] () -> rr_pointer { return std::make_shared<chen::dns::GID>(); }},
            {TypeUNSPEC, [] () -> rr_pointer { return std::make_shared<chen::dns::UNSPEC>(); }},
            {TypeNID, [] () -> rr_pointer { return std::make_shared<chen::dns::NID>(); }},
            {TypeL32, [] () -> rr_pointer { return std::make_shared<chen::dns::L32>(); }},
            {TypeL64, [] () -> rr_pointer { return std::make_shared<chen::dns::L64>(); }},
            {TypeLP, [] () -> rr_pointer { return std::make_shared<chen::dns::LP>(); }},
            {TypeEUI48, [] () -> rr_pointer { return std::make_shared<chen::dns::EUI48>(); }},
            {TypeEUI64, [] () -> rr_pointer { return std::make_shared<chen::dns::EUI64>(); }},
            {TypeTKEY, [] () -> rr_pointer { return std::make_shared<chen::dns::TKEY>(); }},
            {TypeTSIG, [] () -> rr_pointer { return std::make_shared<chen::dns::TSIG>(); }},

            {TypeURI, [] () -> rr_pointer { return std::make_shared<chen::dns::URI>(); }},
            {TypeCAA, [] () -> rr_pointer { return std::make_shared<chen::dns::CAA>(); }},
            {TypeTA, [] () -> rr_pointer { return std::make_shared<chen::dns::TA>(); }},
            {TypeDLV, [] () -> rr_pointer { return std::make_shared<chen::dns::DLV>(); }}
    };

    std::map<OptCode, chen::dns::table::opt_build_type> g_opt_build {
            {OptLLQ, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::LLQ>(); }},
            {OptUL, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::UL>(); }},
            {OptNSID, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::NSID>(); }},
            {OptDAU, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::DAU>(); }},
            {OptDHU, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::DHU>(); }},
            {OptN3U, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::N3U>(); }},
            {OptSubnet, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::Subnet>(); }},
            {OptEXPIRE, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::EXPIRE>(); }},
            {OptCOOKIE, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::COOKIE>(); }},
            {OptKeepalive, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::Keepalive>(); }},
            {OptPadding, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::Padding>(); }},
            {OptCHAIN, [] () -> opt_pointer { return std::make_shared<chen::dns::edns0::CHAIN>(); }}
    };

    std::map<RRType, std::string> g_rr_type_text = {
            {TypeA, "A"},
            {TypeNS, "NS"},
            {TypeMD, "MD"},
            {TypeMF, "MF"},
            {TypeCNAME, "CNAME"},
            {TypeSOA, "SOA"},
            {TypeMB, "MB"},
            {TypeMG, "MG"},
            {TypeMR, "MR"},
            {TypeNULL, "NULL"},
            {TypeWKS, "WKS"},
            {TypePTR, "PTR"},
            {TypeHINFO, "HINFO"},
            {TypeMINFO, "MINFO"},
            {TypeMX, "MX"},
            {TypeTXT, "TXT"},
            {TypeRP, "RP"},
            {TypeAFSDB, "AFSDB"},
            {TypeX25, "X25"},
            {TypeISDN, "ISDN"},
            {TypeRT, "RT"},
            {TypeNSAP, "NSAP"},
            {TypeNSAPPTR, "NSAP-PTR"},  // NSAP-PTR is the official name
            {TypeSIG, "SIG"},
            {TypeKEY, "KEY"},
            {TypePX, "PX"},
            {TypeGPOS, "GPOS"},
            {TypeAAAA, "AAAA"},
            {TypeLOC, "LOC"},
            {TypeNXT, "NXT"},
            {TypeEID, "EID"},
            {TypeNIMLOC, "NIMLOC"},
            {TypeSRV, "SRV"},
            {TypeATMA, "ATMA"},
            {TypeNAPTR, "NAPTR"},
            {TypeKX, "KX"},
            {TypeCERT, "CERT"},
            {TypeA6, "A6"},
            {TypeDNAME, "DNAME"},
            {TypeSINK, "SINK"},
            {TypeOPT, "OPT"},
            {TypeDS, "DS"},
            {TypeSSHFP, "SSHFP"},
            {TypeIPSECKEY, "IPSECKEY"},
            {TypeRRSIG, "RRSIG"},
            {TypeNSEC, "NSEC"},
            {TypeDNSKEY, "DNSKEY"},
            {TypeDHCID, "DHCID"},
            {TypeNSEC3, "NSEC3"},
            {TypeNSEC3PARAM, "NSEC3PARAM"},
            {TypeTLSA, "TLSA"},
            {TypeSMIMEA, "SMIMEA"},
            {TypeHIP, "HIP"},
            {TypeNINFO, "NINFO"},
            {TypeRKEY, "RKEY"},
            {TypeTALINK, "TALINK"},
            {TypeCDS, "CDS"},
            {TypeCDNSKEY, "CDNSKEY"},
            {TypeOPENPGPKEY, "OPENPGPKEY"},
            {TypeCSYNC, "CSYNC"},
            {TypeSPF, "SPF"},
            {TypeUINFO, "UINFO"},
            {TypeUID, "UID"},
            {TypeGID, "GID"},
            {TypeUNSPEC, "UNSPEC"},
            {TypeNID, "NID"},
            {TypeL32, "L32"},
            {TypeL64, "L64"},
            {TypeLP, "LP"},
            {TypeEUI48, "EUI48"},
            {TypeEUI64, "EUI64"},
            {TypeTKEY, "TKEY"},
            {TypeTSIG, "TSIG"},

            {TypeIXFR, "IXFR"},
            {TypeAXFR, "AXFR"},
            {TypeMAILB, "MAILB"},
            {TypeMAILA, "MAILA"},
            {TypeANY, "ANY"},

            {TypeURI, "URI"},
            {TypeCAA, "CAA"},
            {TypeTA, "TA"},
            {TypeDLV, "DLV"}
    };

    std::unordered_map<std::string, RRType> g_rr_text_type;

    std::map<RRClass, std::string> g_rr_class_text = {
            {ClassIN, "IN"},
            {ClassCS, "CS"},
            {ClassCH, "CH"},
            {ClassHS, "HS"},
            {ClassNONE, "NONE"},
            {ClassANY, "ANY"}
    };

    std::unordered_map<std::string, chen::dns::RRClass> g_rr_text_class;

    std::map<QrCode, std::string> g_rr_qr_text = {
            {QrQuery, "QUERY"},
            {QrResponse, "RESPONSE"}
    };

    std::unordered_map<std::string, QrCode> g_rr_text_qr;

    std::map<OpCode, std::string> g_rr_opcode_text = {
            {OpQuery, "QUERY"},
            {OpIQuery, "IQUERY"},
            {OpStatus, "STATUS"},
            {OpNotify, "NOTIFY"},
            {OpUpdate, "UPDATE"}
    };

    std::unordered_map<std::string, OpCode> g_rr_text_opcode;

    std::map<RCode, std::string> g_rr_rcode_text = {
            {RCodeNoError, "NOERROR"},
            {RCodeFormErr, "FORMERR"},
            {RCodeServFail, "SERVFAIL"},
            {RCodeNXDomain, "NXDOMAIN"},
            {RCodeNotImp, "NOTIMPL"},
            {RCodeRefused, "REFUSED"},
            {RCodeYXDomain, "YXDOMAIN"},
            {RCodeYXRrSet, "YXRRSET"},
            {RCodeNXRrSet, "NXRRSET"},
            {RCodeNotAuth, "NOTAUTH"},
            {RCodeNotZone, "NOTZONE"},
            {RCodeBadVers, "BADVERS"},  // BadVers & BadSig has the same value
//        {RCodeBadSig, "BADSIG"},
            {RCodeBadKey, "BADKEY"},
            {RCodeBadTime, "BADTIME"},
            {RCodeBadMode, "BADMODE"},
            {RCodeBadName, "BADNAME"},
            {RCodeBadAlg, "BADALG"},
            {RCodeBadTrunc, "BADTRUNC"},
            {RCodeBadCookie, "BADCOOKIE"},
    };

    std::unordered_map<std::string, RCode> g_rr_text_rcode;

    std::map<OptCode, std::string> g_rr_edns0_text = {
            {OptLLQ, "LLQ"},
            {OptUL, "UL"},
            {OptNSID, "NSID"},
            {OptDAU, "DAU"},
            {OptDHU, "DHU"},
            {OptN3U, "N3U"},
            {OptSubnet, "Subnet"},
            {OptEXPIRE, "EXPIRE"},
            {OptCOOKIE, "COOKIE"},
            {OptKeepalive, "Keepalive"},
            {OptPadding, "Padding"},
            {OptCHAIN, "CHAIN"},
    };

    std::unordered_map<std::string, OptCode> g_rr_text_edns0;

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

chen::dns::table::opt_pointer chen::dns::table::build(edns0::OptCode key)
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
    return it != g_rr_text_type.end() ? it->second : TypeNone;
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
    return it != g_rr_text_class.end() ? it->second : ClassIN;
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
std::string chen::dns::table::opcodeToText(OpCode key)
{
    auto it = g_rr_opcode_text.find(key);
    return it != g_rr_opcode_text.end() ? it->second : "";
}

OpCode chen::dns::table::textToOpcode(const std::string &key)
{
    auto it = g_rr_text_opcode.find(key);
    return it != g_rr_text_opcode.end() ? it->second : OpQuery;
}

// rcode & text
std::string chen::dns::table::rcodeToText(RCode key)
{
    auto it = g_rr_rcode_text.find(key);
    return it != g_rr_rcode_text.end() ? it->second : "";
}

RCode chen::dns::table::textToRcode(const std::string &key)
{
    auto it = g_rr_text_rcode.find(key);
    return it != g_rr_text_rcode.end() ? it->second : RCodeNoError;
}

// edns0 option code
std::string chen::dns::table::edns0ToText(edns0::OptCode key)
{
    auto it = g_rr_edns0_text.find(key);
    return it != g_rr_edns0_text.end() ? it->second : "";
}

chen::dns::edns0::OptCode chen::dns::table::textToEDNS0(const std::string &key)
{
    auto it = g_rr_text_edns0.find(key);
    return it != g_rr_text_edns0.end() ? it->second : edns0::OptNone;
}

// set
void chen::dns::table::set(RRType key, rr_build_type val)
{
    g_rr_build[key] = val;
}

void chen::dns::table::set(edns0::OptCode key, opt_build_type val)
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

void chen::dns::table::set(OpCode key, const std::string &val)
{
    g_rr_opcode_text[key] = val;
    g_rr_text_opcode[val] = key;
}

void chen::dns::table::set(RCode key, const std::string &val)
{
    g_rr_rcode_text[key] = val;
    g_rr_text_rcode[val] = key;
}

void chen::dns::table::set(edns0::OptCode key, const std::string &val)
{
    g_rr_edns0_text[key] = val;
    g_rr_text_edns0[val] = key;
}