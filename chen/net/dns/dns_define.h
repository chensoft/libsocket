/**
 * Created by Jian Chen
 * @since  2015.11.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
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
        enum class QR : std::uint8_t {
            Query    = 0,  // query message
            Response = 1   // response message
        };


        // ---------------------------------------------------------------------
        // OPCODE, 4 bit, kind of query in the message(Operation Code)
        enum class OPCODE : std::uint8_t {
            Query  = 0,  // a standard query
            IQuery = 1,  // an inverse query
            Status = 2,  // a server status request
            // 3-15         reserved for future use
        };


        // ---------------------------------------------------------------------
        // The following bit will treat as bool
        // AA, 1 bit(Authoritative Answer)
        // TC, 1 bit(TrunCation)
        // RD, 1 bit(Recursion Desired)
        // RA, 1 bit(Recursion Available)


        // ---------------------------------------------------------------------
        // Z, 3 bit(Reserved)
        enum class Z : std::uint8_t {
            None = 0
        };


        // ---------------------------------------------------------------------
        // RCODE, 4 bit(Response Code)
        enum class RCODE : std::uint8_t {
            Success        = 0,  // no error
            FormatError    = 1,  // unable to interpret the query
            ServerFailure  = 2,  // unable to process this query due to a problem with the name server
            NameError      = 3,  // domain name referenced in the query does not exist
            NotImplemented = 4,  // does not support the requested kind of query
            Refused        = 5   // the name server refuses to perform the specified operation for policy reasons
            // 6-15                 reserved for future use
        };


        // ---------------------------------------------------------------------
        // Flag mask
        static const std::uint16_t FLAG_MASK_QR     = 0b1000000000000000;
        static const std::uint16_t FLAG_MASK_OPCODE = 0b0111100000000000;
        static const std::uint16_t FLAG_MASK_AA     = 0b0000010000000000;
        static const std::uint16_t FLAG_MASK_TC     = 0b0000001000000000;
        static const std::uint16_t FLAG_MASK_RD     = 0b0000000100000000;
        static const std::uint16_t FLAG_MASK_RA     = 0b0000000010000000;
        static const std::uint16_t FLAG_MASK_Z      = 0b0000000001110000;
        static const std::uint16_t FLAG_MASK_RCODE  = 0b0000000000001111;


        // ---------------------------------------------------------------------
        // RRType(common rr types and question types)
        enum class RRType : std::uint16_t {
            None       = 0,    // none
            A          = 1,    // rfc1035, section 3.4.1, ipv4 address
            NS         = 2,    // rfc1035, section 3.3.11, an authoritative name server
            MD         = 3,    // rfc1035, section 3.3.4, a mail destination (Obsolete - use MX)
            MF         = 4,    // rfc1035, section 3.3.5, a mail forwarder (Obsolete - use MX)
            CNAME      = 5,    // rfc1035, section 3.3.1, the canonical name for an alias
            SOA        = 6,    // rfc1035, section 3.3.13, marks the start of a zone of authority
            MB         = 7,    // rfc1035, section 3.3.3, a mailbox domain name (EXPERIMENTAL)
            MG         = 8,    // rfc1035, section 3.3.6, a mail group member (EXPERIMENTAL)
            MR         = 9,    // rfc1035, section 3.3.8, a mail rename domain name (EXPERIMENTAL)
            RNULL      = 10,   // rfc1035, section 3.3.10, a null RR (EXPERIMENTAL)
            WKS        = 11,   // rfc1035, section 3.4.2, a well known service description
            PTR        = 12,   // rfc1035, section 3.3.12, a domain name pointer
            HINFO      = 13,   // rfc1035, section 3.3.2, host information
            MINFO      = 14,   // rfc1035, section 3.3.7, mailbox or mail list information
            MX         = 15,   // rfc1035, section 3.3.9, mail exchange
            TXT        = 16,   // rfc1035, section 3.3.14, text strings
            RP         = 17,   // rfc1183, section 2.2, responsible person
            AFSDB      = 18,   // rfc1183, section 1, afs data base location
            X25        = 19,   // rfc1183, section 3.1, maps a domain name to a Public Switched Data Network (PSDN) address number
            ISDN       = 20,   // rfc1183, section 3.2, maps a domain name to an ISDN (Integrated Services Digital Network) telephone number
            RT         = 21,   // rfc1183, section 3.3, route through
            NSAP       = 22,   // rfc1706, section 5, map from domain names to NSAPs
            NSAPPTR    = 23,   // rfc1706, section 6, map from NSAPs to domain names under the "NSAP.INT" domain
            SIG        = 24,   // rfc2535, section 4.1, signature
            KEY        = 25,   // rfc2535, section 3.1, key record
            PX         = 26,   // rfc2163, section 4, resource record for MIXER mapping rules
            GPOS       = 27,   // rfc1712, section 3, geographical location
            AAAA       = 28,   // rfc3596, section 2.2, ipv6 address
            LOC        = 29,   // rfc1876, section 2, location record
            NXT        = 30,   // rfc2535, section 5.2, next resource record
            EID        = 31,   // draft-lewis-dns-undocumented-types-01
            NIMLOC     = 32,   // draft-lewis-dns-undocumented-types-01
            SRV        = 33,   // rfc2782, service locator
            ATMA       = 34,   // ATM Name System Specification Version 1.0, maps a domain name to an ATM address
            NAPTR      = 35,   // rfc3403, section 4.1, naming authority pointer
            KX         = 36,   // rfc2230, section 3.1, key exchanger record
            CERT       = 37,   // rfc4398, section 2, certificate record
            DNAME      = 39,   // rfc2672, section 3, delegation name
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
            HIP        = 55,   // rfc5205, section 5, host identity protocol
            NINFO      = 56,   // draft-lewis-dns-undocumented-types-01
            RKEY       = 57,   // draft-lewis-dns-undocumented-types-01
            TALINK     = 58,   // draft-lewis-dns-undocumented-types-01
            CDS        = 59,   // rfc7344, section 3.1, child ds
            CDNSKEY    = 60,   // rfc7344, section 3.2, child DNSKEY
            OPENPGPKEY = 61,   // rfc4880, section 5.5.1.1, OpenPGP key
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

            IXFR  = 251,  // A request for incremental zone transfer
            AXFR  = 252,  // A request for a transfer of an entire zone
            MAILB = 253,  // A request for mailbox-related records (MB, MG or MR)
            MAILA = 254,  // A request for mail agent RRs (Obsolete - see MX)
            ANY   = 255,  // A request for all records

            URI      = 256,    // rfc7553, section 4.5, uri
            CAA      = 257,    // rfc6844, section 5.1.1, certification authority authorization
            TA       = 32768,  // Deploying DNSSEC Without a Signed Root, DNSSEC trust authorities
            DLV      = 32769,  // rfc4431, section 2, DNSSEC lookaside validation
            Reserved = 65535   // reserved
        };


        // ---------------------------------------------------------------------
        // RRClass(common rr class and question class)
        enum class RRClass : std::uint16_t {
            None = 0,  // none

            IN = 1,  // the Internet
            CS = 2,  // the CSNET class (Obsolete - used only for examples in some obsolete RFCs)
            CH = 3,  // the CHAOS class
            HS = 4,  // Hesiod [Dyer 87]

            ANY = 255  // any class
        };


        // ---------------------------------------------------------------------
        // Size limit(rfc1035, section 2.3.4)
        static const std::size_t SIZE_LIMIT_STRING = 255;  // plain text's size
        static const std::size_t SIZE_LIMIT_DOMAIN = 255;  // domain's total size(fqdn)
        static const std::size_t SIZE_LIMIT_LABEL  = 63;   // label's max size
    }
}