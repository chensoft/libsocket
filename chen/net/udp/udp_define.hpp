/**
 * Created by Jian Chen
 * @since  2016.01.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   https://en.wikipedia.org/wiki/User_Datagram_Protocol
 */
#pragma once

namespace chen
{
    namespace udp
    {
        // ---------------------------------------------------------------------
        // Size limit
        static const std::size_t SIZE_LIMIT_PACKET = 65507;  // 65535 − 8 byte UDP header − 20 byte IP header = 65507
    }
}