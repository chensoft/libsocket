/**
 * Created by Jian Chen
 * @since  2015.11.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   https://tools.ietf.org/html/rfc1035
 * @link   https://en.wikipedia.org/wiki/List_of_DNS_record_types
 * @link   http://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml
 */
#pragma once

#include <cstdint>
#include <cstddef>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // QR, 1 bit(Query or Response)
        enum QrCode : std::uint8_t
        {
            QrQuery    = 0,  // query message
            QrResponse = 1   // response message
        };


        // ---------------------------------------------------------------------
        // OPCODE, 4 bit, kind of query in the message(Operation Code)
        enum OpCode : std::uint8_t
        {
            OpQuery  = 0,  // rfc1035, a standard query
            OpIQuery = 1,  // rfc3425, an inverse query(OBSOLETE)
            OpStatus = 2,  // rfc1035, a server status request
            OpNotify = 4,  // rfc1996, notification of zone changes
            OpUpdate = 5,  // rfc2136, dynamic updates
        };


        // ---------------------------------------------------------------------
        // The following bit will treat as bool
        // AA, bit 5, rfc1035, Authoritative Answer
        // TC, bit 6, rfc1035, Truncated Response
        // RD, bit 7, rfc1035, Recursion Desired
        // RA, bit 8, rfc1035, Recursion Available
        // Z,  bit 9, Reserved
        // AD, bit 10, rfc4035, Authentic Data
        // CD, bit 11, rfc4035, Checking Disabled


        // ---------------------------------------------------------------------
        // RCODE, 4 bits response code(rfc1035), 12 bits extended code(rfc6891)
        enum RCode : std::uint8_t
        {
            RCodeNoError   = 0,   // rfc1035, no error
            RCodeFormErr   = 1,   // rfc1035, unable to interpret the query
            RCodeServFail  = 2,   // rfc1035, unable to process this query due to a problem with the name server
            RCodeNXDomain  = 3,   // rfc1035, domain name referenced in the query does not exist
            RCodeNotImp    = 4,   // rfc1035, does not support the requested kind of query
            RCodeRefused   = 5,   // rfc1035, refuses to perform the specified operation for policy reasons
            RCodeYXDomain  = 6,   // rfc2136, some name that ought not to exist, does exist
            RCodeYXRrSet   = 7,   // rfc2136, some RRset that ought not to exist, does exist
            RCodeNXRrSet   = 8,   // rfc2136, some RRset that ought to exist, does not exist
            RCodeNotAuth   = 9,   // rfc2136, the server is not authoritative for the zone named in the Zone Section
            RCodeNotZone   = 10,  // rfc2136, a name used in the Prerequisite or Update Section is not within the zone denoted by the Zone Section

            RCodeBadVers   = 16,  // rfc6891, bad OPT version
            RCodeBadSig    = 16,  // rfc2845, TSIG signature failure, value is the same as BadVers
            RCodeBadKey    = 17,  // rfc2845, key not recognized
            RCodeBadTime   = 18,  // rfc2845, signature out of time window
            RCodeBadMode   = 19,  // rfc2930, bad TKEY mode
            RCodeBadName   = 20,  // rfc2930, duplicate key name
            RCodeBadAlg    = 21,  // rfc2930, algorithm not supported
            RCodeBadTrunc  = 22,  // rfc4635, bad truncation
            RCodeBadCookie = 23   // draft-ietf-dnsop-cookies, bad/missing server cookie
        };


        // ---------------------------------------------------------------------
        // Flag pos & mask
        constexpr std::uint16_t FLAG_POS_QR     = 15;
        constexpr std::uint16_t FLAG_POS_OPCODE = 11;
        constexpr std::uint16_t FLAG_POS_AA     = 10;
        constexpr std::uint16_t FLAG_POS_TC     = 9;
        constexpr std::uint16_t FLAG_POS_RD     = 8;
        constexpr std::uint16_t FLAG_POS_RA     = 7;
        constexpr std::uint16_t FLAG_POS_Z      = 6;
        constexpr std::uint16_t FLAG_POS_AD     = 5;
        constexpr std::uint16_t FLAG_POS_CD     = 4;
        constexpr std::uint16_t FLAG_POS_RCODE  = 0;

        constexpr std::uint16_t FLAG_MASK_QR     = 1 << FLAG_POS_QR;
        constexpr std::uint16_t FLAG_MASK_OPCODE = 0b1111 << FLAG_POS_OPCODE;  // 4 bits
        constexpr std::uint16_t FLAG_MASK_AA     = 1 << FLAG_POS_AA;
        constexpr std::uint16_t FLAG_MASK_TC     = 1 << FLAG_POS_TC;
        constexpr std::uint16_t FLAG_MASK_RD     = 1 << FLAG_POS_RD;
        constexpr std::uint16_t FLAG_MASK_RA     = 1 << FLAG_POS_RA;
        constexpr std::uint16_t FLAG_MASK_Z      = 1 << FLAG_POS_Z;
        constexpr std::uint16_t FLAG_MASK_AD     = 1 << FLAG_POS_AD;
        constexpr std::uint16_t FLAG_MASK_CD     = 1 << FLAG_POS_CD;
        constexpr std::uint16_t FLAG_MASK_RCODE  = 0b1111 << FLAG_POS_RCODE;  // 4 bits


        // ---------------------------------------------------------------------
        // RRType(common rr types and question types)
        enum RRType : std::uint16_t
        {
            TypeNone       = 0,    // none
            TypeA          = 1,    // rfc1035, section 3.4.1, a host address
            TypeNS         = 2,    // rfc1035, section 3.3.11, an authoritative name server
            TypeMD         = 3,    // rfc1035, section 3.3.4, a mail destination (OBSOLETE - use MX)
            TypeMF         = 4,    // rfc1035, section 3.3.5, a mail forwarder (OBSOLETE - use MX)
            TypeCNAME      = 5,    // rfc1035, section 3.3.1, the canonical name for an alias
            TypeSOA        = 6,    // rfc1035, section 3.3.13, marks the start of a zone of authority
            TypeMB         = 7,    // rfc1035, section 3.3.3, a mailbox domain name (EXPERIMENTAL)
            TypeMG         = 8,    // rfc1035, section 3.3.6, a mail group member (EXPERIMENTAL)
            TypeMR         = 9,    // rfc1035, section 3.3.8, a mail rename domain name (EXPERIMENTAL)
            TypeNULL       = 10,   // rfc1035, section 3.3.10, a null rr (EXPERIMENTAL)
            TypeWKS        = 11,   // rfc1035, section 3.4.2, a well known service description
            TypePTR        = 12,   // rfc1035, section 3.3.12, a domain name pointer
            TypeHINFO      = 13,   // rfc1035, section 3.3.2, host information
            TypeMINFO      = 14,   // rfc1035, section 3.3.7, mailbox or mail list information
            TypeMX         = 15,   // rfc1035, section 3.3.9, mail exchange
            TypeTXT        = 16,   // rfc1035, section 3.3.14, text strings
            TypeRP         = 17,   // rfc1183, section 2.2, for Responsible Person
            TypeAFSDB      = 18,   // rfc1183, section 1, for AFS Data Base location
            TypeX25        = 19,   // rfc1183, section 3.1, for X.25 PSDN address
            TypeISDN       = 20,   // rfc1183, section 3.2, for ISDN address
            TypeRT         = 21,   // rfc1183, section 3.3, for Route Through
            TypeNSAP       = 22,   // rfc1706, section 5, for NSAP address, NSAP style A record
            TypeNSAPPTR    = 23,   // rfc1348, for domain name pointer, NSAP style
            TypeSIG        = 24,   // rfc2535, section 4.1, for security signature
            TypeKEY        = 25,   // rfc2535, section 3.1, for security key
            TypePX         = 26,   // rfc2163, section 4, X.400 mail mapping information
            TypeGPOS       = 27,   // rfc1712, section 3, Geographical Position
            TypeAAAA       = 28,   // rfc3596, section 2.2, IP6 Address
            TypeLOC        = 29,   // rfc1876, section 2, Location Information
            TypeNXT        = 30,   // rfc2535, section 5.2, Next Domain (OBSOLETE)
            TypeEID        = 31,   // draft-lewis-dns-undocumented-types-01, Endpoint Identifier
            TypeNIMLOC     = 32,   // draft-lewis-dns-undocumented-types-01, Nimrod Locator
            TypeSRV        = 33,   // rfc2782, Server Selection
            TypeATMA       = 34,   // ATM Name System Specification Version 1.0, ATM Address
            TypeNAPTR      = 35,   // rfc3403, section 4.1, Naming Authority Pointer
            TypeKX         = 36,   // rfc2230, section 3.1, Key Exchanger
            TypeCERT       = 37,   // rfc4398, section 2, CERT
            TypeA6         = 38,   // A6 (OBSOLETE - use AAAA)
            TypeDNAME      = 39,   // rfc2672, section 3, delegation name
            TypeSINK       = 40,   // draft-eastlake-kitchen-sink-02, kitchen sink resource record
            TypeOPT        = 41,   // rfc6891, section 6.1.2, option, a pseudo-record type
            TypeDS         = 43,   // rfc4034, section 5.1, delegation signer
            TypeSSHFP      = 44,   // rfc4255, section 3.1, SSH public key fingerprint
            TypeIPSECKEY   = 45,   // rfc4025, section 2.1, IPsec key
            TypeRRSIG      = 46,   // rfc4034, section 3.1, DNSSEC signature
            TypeNSEC       = 47,   // rfc4034, section 4.1, next-secure record
            TypeDNSKEY     = 48,   // rfc4034, section 2.1, DNS key record
            TypeDHCID      = 49,   // rfc4701, section 3.1, DHCP identifier
            TypeNSEC3      = 50,   // rfc5155, section 3.2, NSEC record version 3
            TypeNSEC3PARAM = 51,   // rfc5155, section 4.2, NSEC record version 3
            TypeTLSA       = 52,   // rfc6698, section 2.1, TLSA certificate association
            TypeSMIMEA     = 53,   // draft-ietf-dane-smime, section 2, S/MIME cert association
            TypeHIP        = 55,   // rfc5205, section 5, host identity protocol
            TypeNINFO      = 56,   // draft-lewis-dns-undocumented-types-01
            TypeRKEY       = 57,   // draft-lewis-dns-undocumented-types-01
            TypeTALINK     = 58,   // draft-lewis-dns-undocumented-types-01, Trust Anchor LINK
            TypeCDS        = 59,   // rfc7344, section 3.1, child ds
            TypeCDNSKEY    = 60,   // rfc7344, section 3.2, DNSKEY(s) the Child wants reflected in DS
            TypeOPENPGPKEY = 61,   // draft-ietf-dane-openpgpkey, section 2.1, OpenPGP key
            TypeCSYNC      = 62,   // rfc7477, section 2.1.1, Child-To-Parent Synchronization
            TypeSPF        = 99,   // rfc4408, section 3, declares which hosts are, and are not, authorized to use a domain name for the "HELO" and "MAIL FROM" identities
            TypeUINFO      = 100,  // IANA reserved, no RFC documented
            TypeUID        = 101,  // IANA reserved, no RFC documented
            TypeGID        = 102,  // IANA reserved, no RFC documented
            TypeUNSPEC     = 103,  // IANA reserved, no RFC documented
            TypeNID        = 104,  // rfc6742, section 2.1.1, hold values for Node Identifiers that will be used for ILNP-capable nodes
            TypeL32        = 105,  // rfc6742, section 2.2.1, hold 32-bit Locator values for ILNPv4-capable nodes
            TypeL64        = 106,  // rfc6742, section 2.3.1, hold unsigned 64-bit Locator values for ILNPv6-capable nodes
            TypeLP         = 107,  // rfc6742, section 2.4.1, hold the name of a subnetwork for ILNP
            TypeEUI48      = 108,  // rfc7043, section 3.1, store a single EUI-48 address in the DNS
            TypeEUI64      = 109,  // rfc7043, section 4.1, store a single EUI-64 address in the DNS
            TypeTKEY       = 249,  // rfc2930, section 2, transaction key
            TypeTSIG       = 250,  // rfc2845, section 2.3, transaction signature

            TypeIXFR       = 251,  // rfc1995, A request for incremental zone transfer
            TypeAXFR       = 252,  // rfc1035, A request for a transfer of an entire zone
            TypeMAILB      = 253,  // rfc1035, A request for mailbox-related records (MB, MG or MR)
            TypeMAILA      = 254,  // rfc1035, A request for mail agent rrs (Obsolete - see MX)
            TypeANY        = 255,  // rfc1035, A request for all records

            TypeURI        = 256,    // rfc7553, section 4.5, uri
            TypeCAA        = 257,    // rfc6844, section 5.1.1, certification authority authorization
            TypeTA         = 32768,  // Deploying DNSSEC Without a Signed Root, DNSSEC trust authorities
            TypeDLV        = 32769,  // rfc4431, section 2, DNSSEC lookaside validation
            TypeReserved   = 65535   // reserved
        };


        // ---------------------------------------------------------------------
        // RRClass(common rr class and question class)
        enum RRClass : std::uint16_t
        {
            ClassIN   = 1,  // rfc1035, the Internet
            ClassCS   = 2,  // rfc1035, the CSNET class (Obsolete - used only for examples in some obsolete RFCs)
            ClassCH   = 3,  // rfc1035, the CHAOS class
            ClassHS   = 4,  // rfc1035, Hesiod [Dyer 87]

            ClassNONE = 254,  // rfc2136, name is not in use or rrset does not exist
            ClassANY  = 255   // rfc1035, any class
        };


        // ---------------------------------------------------------------------
        // Size limit(rfc1035, section 2.3.4)
        constexpr std::size_t SIZE_LIMIT_STRING = 255;  // plain text's size
        constexpr std::size_t SIZE_LIMIT_DOMAIN = 255;  // domain's total size(fqdn)
        constexpr std::size_t SIZE_LIMIT_LABEL  = 63;   // label's max size


        // ---------------------------------------------------------------------
        // EDNS0
        namespace edns0
        {
            // -----------------------------------------------------------------
            // Option Code, 2 bytes
            // 0, 4: Reserved
            // 14 ~ 65000: Unassigned
            // 65001 ~ 65534: Reserved for Local/Experimental Use
            // 65535: Reserved for future expansion
            // @link http://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml
            enum OptCode : std::uint16_t
            {
                OptNone      = 0,   // none
                OptLLQ       = 1,   // http://files.dns-sd.org/draft-sekar-dns-llq.txt, DNS Long-Lived Queries
                OptUL        = 2,   // http://files.dns-sd.org/draft-sekar-dns-ul.txt, Dynamic DNS Update Leases
                OptNSID      = 3,   // rfc5001, DNS Name Server Identifier (NSID) Option
                OptDAU       = 5,   // rfc6975, Signaling DNSSEC Algorithm Understood
                OptDHU       = 6,   // rfc6975, DS Hash Understood
                OptN3U       = 7,   // rfc6975, NSEC3 Hash Understood
                OptSubnet    = 8,   // rfc7871, Client Subnet in DNS Queries
                OptEXPIRE    = 9,   // rfc7314, Extension Mechanisms for DNS (EDNS) EXPIRE Option
                OptCOOKIE    = 10,  // rfc7873, Domain Name System (DNS) Cookies
                OptKeepalive = 11,  // rfc7828, The edns-tcp-keepalive EDNS0 Option
                OptPadding   = 12,  // rfc7830, The EDNS(0) Padding Option
                OptCHAIN     = 13,  // rfc7901, CHAIN Query Requests in DNS
            };
        }
    }
}