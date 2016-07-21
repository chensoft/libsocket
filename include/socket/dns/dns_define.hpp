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
        // QR Code, 1 bit(Query or Response)
        enum class QR : std::uint8_t
        {
            Query    = 0,  // query message
            Response = 1   // response message
        };


        // ---------------------------------------------------------------------
        // OPCODE, 4 bit, kind of query in the message(Operation Code)
        enum class OPCODE : std::uint8_t
        {
            Query  = 0,  // rfc1035, a standard query
            IQuery = 1,  // rfc3425, an inverse query(OBSOLETE)
            Status = 2,  // rfc1035, a server status request
            Notify = 4,  // rfc1996, notification of zone changes
            Update = 5,  // rfc2136, dynamic updates
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
        // RCODE, 4 bit(Response Code)
        enum class RCODE : std::uint8_t
        {
            NoError   = 0,   // rfc1035, no error
            FormErr   = 1,   // rfc1035, unable to interpret the query
            ServFail  = 2,   // rfc1035, unable to process this query due to a problem with the name server
            NXDomain  = 3,   // rfc1035, domain name referenced in the query does not exist
            NotImp    = 4,   // rfc1035, does not support the requested kind of query
            Refused   = 5,   // rfc1035, refuses to perform the specified operation for policy reasons
            YXDomain  = 6,   // rfc2136, some name that ought not to exist, does exist
            YXRrSet   = 7,   // rfc2136, some RRset that ought not to exist, does exist
            NXRrSet   = 8,   // rfc2136, some RRset that ought to exist, does not exist
            NotAuth   = 9,   // rfc2136, the server is not authoritative for the zone named in the Zone Section
            NotZone   = 10,  // rfc2136, a name used in the Prerequisite or Update Section is not within the zone denoted by the Zone Section
            BadVers   = 16,  // rfc6891, bad OPT version
            BadSig    = 16,  // rfc2845, TSIG signature failure, same value as BadVers
            BadKey    = 17,  // rfc2845, key not recognized
            BadTime   = 18,  // rfc2845, signature out of time window
            BadMode   = 19,  // rfc2930, bad TKEY mode
            BadName   = 20,  // rfc2930, duplicate key name
            BadAlg    = 21,  // rfc2930, algorithm not supported
            BadTrunc  = 22,  // rfc4635, bad truncation
            BadCookie = 23   // draft-ietf-dnsop-cookies, bad/missing server cookie
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
        enum class RRType : std::uint16_t
        {
            None       = 0,    // none
            A          = 1,    // rfc1035, section 3.4.1, a host address
            NS         = 2,    // rfc1035, section 3.3.11, an authoritative name server
            MD         = 3,    // rfc1035, section 3.3.4, a mail destination (OBSOLETE - use MX)
            MF         = 4,    // rfc1035, section 3.3.5, a mail forwarder (OBSOLETE - use MX)
            CNAME      = 5,    // rfc1035, section 3.3.1, the canonical name for an alias
            SOA        = 6,    // rfc1035, section 3.3.13, marks the start of a zone of authority
            MB         = 7,    // rfc1035, section 3.3.3, a mailbox domain name (EXPERIMENTAL)
            MG         = 8,    // rfc1035, section 3.3.6, a mail group member (EXPERIMENTAL)
            MR         = 9,    // rfc1035, section 3.3.8, a mail rename domain name (EXPERIMENTAL)
            NUL        = 10,   // rfc1035, section 3.3.10, a null RR (EXPERIMENTAL)
            WKS        = 11,   // rfc1035, section 3.4.2, a well known service description
            PTR        = 12,   // rfc1035, section 3.3.12, a domain name pointer
            HINFO      = 13,   // rfc1035, section 3.3.2, host information
            MINFO      = 14,   // rfc1035, section 3.3.7, mailbox or mail list information
            MX         = 15,   // rfc1035, section 3.3.9, mail exchange
            TXT        = 16,   // rfc1035, section 3.3.14, text strings
            RP         = 17,   // rfc1183, section 2.2, for Responsible Person
            AFSDB      = 18,   // rfc1183, section 1, for AFS Data Base location
            X25        = 19,   // rfc1183, section 3.1, for X.25 PSDN address
            ISDN       = 20,   // rfc1183, section 3.2, for ISDN address
            RT         = 21,   // rfc1183, section 3.3, for Route Through
            NSAP       = 22,   // rfc1706, section 5, for NSAP address, NSAP style A record
            NSAPPTR    = 23,   // rfc1348, for domain name pointer, NSAP style
            SIG        = 24,   // rfc2535, section 4.1, for security signature
            KEY        = 25,   // rfc2535, section 3.1, for security key
            PX         = 26,   // rfc2163, section 4, X.400 mail mapping information
            GPOS       = 27,   // rfc1712, section 3, Geographical Position
            AAAA       = 28,   // rfc3596, section 2.2, IP6 Address
            LOC        = 29,   // rfc1876, section 2, Location Information
            NXT        = 30,   // rfc2535, section 5.2, Next Domain (OBSOLETE)
            EID        = 31,   // draft-lewis-dns-undocumented-types-01, Endpoint Identifier
            NIMLOC     = 32,   // draft-lewis-dns-undocumented-types-01, Nimrod Locator
            SRV        = 33,   // rfc2782, Server Selection
            ATMA       = 34,   // ATM Name System Specification Version 1.0, ATM Address
            NAPTR      = 35,   // rfc3403, section 4.1, Naming Authority Pointer
            KX         = 36,   // rfc2230, section 3.1, Key Exchanger
            CERT       = 37,   // rfc4398, section 2, CERT
            A6         = 38,   // A6 (OBSOLETE - use AAAA)
            DNAME      = 39,   // rfc2672, section 3, delegation name
            SINK       = 40,   // draft-eastlake-kitchen-sink-02, kitchen sink resource record
            OPT        = 41,   // rfc6891, section 6.1.2, option, a pseudo-record type
            DS         = 43,   // rfc4034, section 5.1, delegation signer
            SSHFP      = 44,   // rfc4255, section 3.1, SSH public key fingerprint
            IPSECKEY   = 45,   // rfc4025, section 2.1, IPsec key
            RRSIG      = 46,   // rfc4034, section 3.1, DNSSEC signature
            NSEC       = 47,   // rfc4034, section 4.1, next-secure record
            DNSKEY     = 48,   // rfc4034, section 2.1, DNS key record
            DHCID      = 49,   // rfc4701, section 3.1, DHCP identifier
            NSEC3      = 50,   // rfc5155, section 3.2, NSEC record version 3
            NSEC3PARAM = 51,   // rfc5155, section 4.2, NSEC record version 3
            TLSA       = 52,   // rfc6698, section 2.1, TLSA certificate association
            SMIMEA     = 53,   // draft-ietf-dane-smime, section 2, S/MIME cert association
            HIP        = 55,   // rfc5205, section 5, host identity protocol
            NINFO      = 56,   // draft-lewis-dns-undocumented-types-01
            RKEY       = 57,   // draft-lewis-dns-undocumented-types-01
            TALINK     = 58,   // draft-lewis-dns-undocumented-types-01, Trust Anchor LINK
            CDS        = 59,   // rfc7344, section 3.1, child ds
            CDNSKEY    = 60,   // rfc7344, section 3.2, DNSKEY(s) the Child wants reflected in DS
            OPENPGPKEY = 61,   // draft-ietf-dane-openpgpkey, section 2.1, OpenPGP key
            CSYNC      = 62,   // rfc7477, section 2.1.1, Child-To-Parent Synchronization
            SPF        = 99,   // rfc4408, section 3, declares which hosts are, and are not, authorized to use a domain name for the "HELO" and "MAIL FROM" identities
            UINFO      = 100,  // IANA reserved, no RFC documented
            UID        = 101,  // IANA reserved, no RFC documented
            GID        = 102,  // IANA reserved, no RFC documented
            UNSPEC     = 103,  // IANA reserved, no RFC documented
            NID        = 104,  // rfc6742, section 2.1.1, hold values for Node Identifiers that will be used for ILNP-capable nodes
            L32        = 105,  // rfc6742, section 2.2.1, hold 32-bit Locator values for ILNPv4-capable nodes
            L64        = 106,  // rfc6742, section 2.3.1, hold unsigned 64-bit Locator values for ILNPv6-capable nodes
            LP         = 107,  // rfc6742, section 2.4.1, hold the name of a subnetwork for ILNP
            EUI48      = 108,  // rfc7043, section 3.1, store a single EUI-48 address in the DNS
            EUI64      = 109,  // rfc7043, section 4.1, store a single EUI-64 address in the DNS
            TKEY       = 249,  // rfc2930, section 2, transaction key
            TSIG       = 250,  // rfc2845, section 2.3, transaction signature

            IXFR  = 251,  // rfc1995, A request for incremental zone transfer
            AXFR  = 252,  // rfc1035, A request for a transfer of an entire zone
            MAILB = 253,  // rfc1035, A request for mailbox-related records (MB, MG or MR)
            MAILA = 254,  // rfc1035, A request for mail agent RRs (Obsolete - see MX)
            ANY   = 255,  // rfc1035, A request for all records

            URI      = 256,    // rfc7553, section 4.5, uri
            CAA      = 257,    // rfc6844, section 5.1.1, certification authority authorization
            TA       = 32768,  // Deploying DNSSEC Without a Signed Root, DNSSEC trust authorities
            DLV      = 32769,  // rfc4431, section 2, DNSSEC lookaside validation
            Reserved = 65535   // reserved
        };


        // ---------------------------------------------------------------------
        // RRClass(common rr class and question class)
        enum class RRClass : std::uint16_t
        {
            IN = 1,  // rfc1035, the Internet
            CS = 2,  // rfc1035, the CSNET class (Obsolete - used only for examples in some obsolete RFCs)
            CH = 3,  // rfc1035, the CHAOS class
            HS = 4,  // rfc1035, Hesiod [Dyer 87]

            NONE = 254,  // rfc2136, name is not in use or rrset does not exist
            ANY  = 255   // rfc1035, any class
        };


        // ---------------------------------------------------------------------
        // Size limit(rfc1035, section 2.3.4)
        constexpr std::size_t SIZE_LIMIT_STRING = 255;  // plain text's size
        constexpr std::size_t SIZE_LIMIT_DOMAIN = 255;  // domain's total size(fqdn)
        constexpr std::size_t SIZE_LIMIT_LABEL  = 63;   // label's max size
    }
}