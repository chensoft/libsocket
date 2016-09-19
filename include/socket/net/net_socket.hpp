/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/bsd_socket.hpp>

namespace chen
{
    namespace net
    {
        class socket
        {
        public:
            virtual ~socket() = default;

        protected:
            friend class proactor;

            /**
             * Internal socket
             */
            bsd::socket& handle();

            /**
             * Event callbacks
             */
            virtual void onEventSend(std::size_t size, std::error_code error) = 0;
            virtual void onEventRecv(std::vector<std::uint8_t> data, std::error_code error) = 0;
            virtual void onEventEOF() = 0;  // connection refused, disconnect or other error
        };
    }
}