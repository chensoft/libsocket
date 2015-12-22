/**
 * Created by Jian Chen
 * @since  2015.11.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 * @link   https://tools.ietf.org/html/rfc1035
 * @link   https://en.wikipedia.org/wiki/List_of_DNS_record_types
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
            None       = 0,   // none
            A          = 1,   // rfc1035, section 3.4.1, a host address
            NS         = 2,   // rfc1035, section 3.3.11, an authoritative name server
            MD         = 3,   // rfc1035, section 3.3.4, a mail destination (Obsolete - use MX)
            MF         = 4,   // rfc1035, section 3.3.5, a mail forwarder (Obsolete - use MX)
            CNAME      = 5,   // rfc1035, section 3.3.1, the canonical name for an alias
            SOA        = 6,   // rfc1035, section 3.3.13, marks the start of a zone of authority
            MB         = 7,   // rfc1035, section 3.3.3, a mailbox domain name (EXPERIMENTAL)
            MG         = 8,   // rfc1035, section 3.3.6, a mail group member (EXPERIMENTAL)
            MR         = 9,   // rfc1035, section 3.3.8, a mail rename domain name (EXPERIMENTAL)
            RNULL      = 10,  // rfc1035, section 3.3.10, a null RR (EXPERIMENTAL)
            WKS        = 11,  // rfc1035, section 3.4.2, a well known service description
            PTR        = 12,  // rfc1035, section 3.3.12, a domain name pointer
            HINFO      = 13,  // rfc1035, section 3.3.2, host information
            MINFO      = 14,  // rfc1035, section 3.3.7, mailbox or mail list information
            MX         = 15,  // rfc1035, section 3.3.9, mail exchange
            TXT        = 16,  // rfc1035, section 3.3.14, text strings

            AXFR  = 252,  // A request for a transfer of an entire zone
            MAILB = 253,  // A request for mailbox-related records (MB, MG or MR)
            MAILA = 254,  // A request for mail agent RRs (Obsolete - see MX)
            ANY   = 255   // A request for all records
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