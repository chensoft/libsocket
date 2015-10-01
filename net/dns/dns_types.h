/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 * @link   https://tools.ietf.org/html/rfc1035
 */
#pragma once

#include <cstdint>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // QR Code, 1 bit
        enum class QR : std::uint8_t {
            Query    = 0,  // query message
            Response = 1   // response message
        };


        // ---------------------------------------------------------------------
        // OPCODE, 4 bit, kind of query in the message
        enum class OPCODE : std::uint8_t {
            Query  = 0,  // a standard query
            IQuery = 1,  // an inverse query
            Status = 2,  // a server status request
            // 3-15         reserved for future use
        };


        // ---------------------------------------------------------------------
        // AA, 1 bit, whether it's a authoritative answer
        enum class AA : std::uint8_t {
            No  = 0,
            Yes = 1
        };


        // ---------------------------------------------------------------------
        // TC, 1 bit, whether the message can be truncated
        enum class TC : std::uint8_t {
            No  = 0,
            Yes = 1
        };


        // ---------------------------------------------------------------------
        // RD, 1 bit, want the server process query recursively
        enum class RD : std::uint8_t {
            No  = 0,
            Yes = 1
        };


        // ---------------------------------------------------------------------
        // RA, 1 bit, whether recursive query support is available
        enum class RA : std::uint8_t {
            No  = 0,
            Yes = 1
        };


        // ---------------------------------------------------------------------
        // Z, 3 bit, reserved
        enum class Z : std::uint8_t {
            None = 0
        };


        // ---------------------------------------------------------------------
        // RCODE, 4 bit, response code
        enum class RCODE : std::uint8_t {
            Success        = 0,  // no error
            FormatError    = 1,  // unable to interpret the query
            ServerFailure  = 2,  // unable to process this query due to a problem with the name server
            NameError      = 3,  // domain name referenced in the query does not exist
            NotImplemented = 4,  // does not support the requested kind of query
            Refused        = 5   // the name server refuses to perform the specified operation for policy reasons
            // 6-15                 reserved for future use
        };
    }
}