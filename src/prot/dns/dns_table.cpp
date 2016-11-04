/**
 * Created by Jian Chen
 * @since  2015.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/prot/dns/dns_table.hpp>
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
            {TypeA, [] () -> rr_pointer { return std::make_shared<rr_a>(); }},
            {TypeNS, [] () -> rr_pointer { return std::make_shared<rr_ns>(); }},
            {TypeMD, [] () -> rr_pointer { return std::make_shared<rr_md>(); }},
            {TypeMF, [] () -> rr_pointer { return std::make_shared<rr_mf>(); }},
            {TypeCNAME, [] () -> rr_pointer { return std::make_shared<rr_cname>(); }},
            {TypeSOA, [] () -> rr_pointer { return std::make_shared<rr_soa>(); }},
            {TypeMB, [] () -> rr_pointer { return std::make_shared<rr_mb>(); }},
            {TypeMG, [] () -> rr_pointer { return std::make_shared<rr_mg>(); }},
            {TypeMR, [] () -> rr_pointer { return std::make_shared<rr_mr>(); }},
            {TypeNULL, [] () -> rr_pointer { return std::make_shared<rr_null>(); }},
            {TypeWKS, [] () -> rr_pointer { return std::make_shared<rr_wks>(); }},
            {TypePTR, [] () -> rr_pointer { return std::make_shared<rr_ptr>(); }},
            {TypeHINFO, [] () -> rr_pointer { return std::make_shared<rr_hinfo>(); }},
            {TypeMINFO, [] () -> rr_pointer { return std::make_shared<rr_minfo>(); }},
            {TypeMX, [] () -> rr_pointer { return std::make_shared<rr_mx>(); }},
            {TypeTXT, [] () -> rr_pointer { return std::make_shared<rr_txt>(); }},
            {TypeRP, [] () -> rr_pointer { return std::make_shared<rr_rp>(); }},
            {TypeAFSDB, [] () -> rr_pointer { return std::make_shared<rr_afsdb>(); }},
            {TypeX25, [] () -> rr_pointer { return std::make_shared<rr_x25>(); }},
            {TypeISDN, [] () -> rr_pointer { return std::make_shared<rr_isdn>(); }},
            {TypeRT, [] () -> rr_pointer { return std::make_shared<rr_rt>(); }},
            {TypeNSAP, [] () -> rr_pointer { return std::make_shared<rr_nsap>(); }},
            {TypeNSAPPTR, [] () -> rr_pointer { return std::make_shared<rr_nsapptr>(); }},
            {TypeSIG, [] () -> rr_pointer { return std::make_shared<rr_sig>(); }},
            {TypeKEY, [] () -> rr_pointer { return std::make_shared<rr_key>(); }},
            {TypePX, [] () -> rr_pointer { return std::make_shared<rr_px>(); }},
            {TypeGPOS, [] () -> rr_pointer { return std::make_shared<rr_gpos>(); }},
            {TypeAAAA, [] () -> rr_pointer { return std::make_shared<rr_aaaa>(); }},
            {TypeLOC, [] () -> rr_pointer { return std::make_shared<rr_loc>(); }},
            {TypeNXT, [] () -> rr_pointer { return std::make_shared<rr_nxt>(); }},
            {TypeEID, [] () -> rr_pointer { return std::make_shared<rr_eid>(); }},
            {TypeNIMLOC, [] () -> rr_pointer { return std::make_shared<rr_nimloc>(); }},
            {TypeSRV, [] () -> rr_pointer { return std::make_shared<rr_srv>(); }},
            {TypeATMA, [] () -> rr_pointer { return std::make_shared<rr_atma>(); }},
            {TypeNAPTR, [] () -> rr_pointer { return std::make_shared<rr_naptr>(); }},
            {TypeKX, [] () -> rr_pointer { return std::make_shared<rr_kx>(); }},
            {TypeCERT, [] () -> rr_pointer { return std::make_shared<rr_cert>(); }},
            {TypeA6, [] () -> rr_pointer { return std::make_shared<rr_a6>(); }},
            {TypeDNAME, [] () -> rr_pointer { return std::make_shared<rr_dname>(); }},
            {TypeSINK, [] () -> rr_pointer { return std::make_shared<rr_sink>(); }},
            {TypeOPT, [] () -> rr_pointer { return std::make_shared<rr_opt>(); }},
            {TypeDS, [] () -> rr_pointer { return std::make_shared<rr_ds>(); }},
            {TypeSSHFP, [] () -> rr_pointer { return std::make_shared<rr_sshfp>(); }},
            {TypeIPSECKEY, [] () -> rr_pointer { return std::make_shared<rr_ipseckey>(); }},
            {TypeRRSIG, [] () -> rr_pointer { return std::make_shared<rr_rrsig>(); }},
            {TypeNSEC, [] () -> rr_pointer { return std::make_shared<rr_nsec>(); }},
            {TypeDNSKEY, [] () -> rr_pointer { return std::make_shared<rr_dnskey>(); }},
            {TypeDHCID, [] () -> rr_pointer { return std::make_shared<rr_dhcid>(); }},
            {TypeNSEC3, [] () -> rr_pointer { return std::make_shared<rr_nsec3>(); }},
            {TypeNSEC3PARAM, [] () -> rr_pointer { return std::make_shared<rr_nsec3param>(); }},
            {TypeTLSA, [] () -> rr_pointer { return std::make_shared<rr_tlsa>(); }},
            {TypeSMIMEA, [] () -> rr_pointer { return std::make_shared<rr_smimea>(); }},
            {TypeHIP, [] () -> rr_pointer { return std::make_shared<rr_hip>(); }},
            {TypeNINFO, [] () -> rr_pointer { return std::make_shared<rr_ninfo>(); }},
            {TypeRKEY, [] () -> rr_pointer { return std::make_shared<rr_rkey>(); }},
            {TypeTALINK, [] () -> rr_pointer { return std::make_shared<rr_talink>(); }},
            {TypeCDS, [] () -> rr_pointer { return std::make_shared<rr_cds>(); }},
            {TypeCDNSKEY, [] () -> rr_pointer { return std::make_shared<rr_cdnskey>(); }},
            {TypeOPENPGPKEY, [] () -> rr_pointer { return std::make_shared<rr_openpgpkey>(); }},
            {TypeCSYNC, [] () -> rr_pointer { return std::make_shared<rr_csync>(); }},
            {TypeSPF, [] () -> rr_pointer { return std::make_shared<rr_spf>(); }},
            {TypeUINFO, [] () -> rr_pointer { return std::make_shared<rr_uinfo>(); }},
            {TypeUID, [] () -> rr_pointer { return std::make_shared<rr_uid>(); }},
            {TypeGID, [] () -> rr_pointer { return std::make_shared<rr_gid>(); }},
            {TypeUNSPEC, [] () -> rr_pointer { return std::make_shared<rr_unspec>(); }},
            {TypeNID, [] () -> rr_pointer { return std::make_shared<rr_nid>(); }},
            {TypeL32, [] () -> rr_pointer { return std::make_shared<rr_l32>(); }},
            {TypeL64, [] () -> rr_pointer { return std::make_shared<rr_l64>(); }},
            {TypeLP, [] () -> rr_pointer { return std::make_shared<rr_lp>(); }},
            {TypeEUI48, [] () -> rr_pointer { return std::make_shared<rr_eui48>(); }},
            {TypeEUI64, [] () -> rr_pointer { return std::make_shared<rr_eui64>(); }},
            {TypeTKEY, [] () -> rr_pointer { return std::make_shared<rr_tkey>(); }},
            {TypeTSIG, [] () -> rr_pointer { return std::make_shared<rr_tsig>(); }},

            {TypeURI, [] () -> rr_pointer { return std::make_shared<rr_uri>(); }},
            {TypeCAA, [] () -> rr_pointer { return std::make_shared<rr_caa>(); }},
            {TypeTA, [] () -> rr_pointer { return std::make_shared<rr_ta>(); }},
            {TypeDLV, [] () -> rr_pointer { return std::make_shared<rr_dlv>(); }}
    };

    std::map<OptCode, chen::dns::table::opt_build_type> g_opt_build {
            {OptLLQ, [] () -> opt_pointer { return std::make_shared<opt_llq>(); }},
            {OptUL, [] () -> opt_pointer { return std::make_shared<opt_ul>(); }},
            {OptNSID, [] () -> opt_pointer { return std::make_shared<opt_nsid>(); }},
            {OptDAU, [] () -> opt_pointer { return std::make_shared<opt_dau>(); }},
            {OptDHU, [] () -> opt_pointer { return std::make_shared<opt_dhu>(); }},
            {OptN3U, [] () -> opt_pointer { return std::make_shared<opt_n3u>(); }},
            {OptSubnet, [] () -> opt_pointer { return std::make_shared<opt_subnet>(); }},
            {OptEXPIRE, [] () -> opt_pointer { return std::make_shared<opt_expire>(); }},
            {OptCOOKIE, [] () -> opt_pointer { return std::make_shared<opt_cookie>(); }},
            {OptKeepalive, [] () -> opt_pointer { return std::make_shared<opt_keepalive>(); }},
            {OptPadding, [] () -> opt_pointer { return std::make_shared<opt_padding>(); }},
            {OptCHAIN, [] () -> opt_pointer { return std::make_shared<opt_chain>(); }}
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