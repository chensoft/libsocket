/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * -----------------------------------------------------------------------------
 * Notice:
 * By default OS X has limited the maximum UDP-package to be 9216 bytes, see:
 * http://stackoverflow.com/questions/22819214/udp-message-too-long
 * You can alter this limit by: sudo sysctl -w net.inet.udp.maxdgram=65507
 */
#pragma once

#include <socket/so/so_socket.hpp>

namespace chen
{
    namespace udp
    {
        class socket : public chen::so::socket
        {
        public:
            socket();
            explicit socket(void *so);

        public:
            /**
             * Send packet to remote
             */
            void send(const void *data, std::size_t size,
                      const std::string &addr, std::uint16_t port,
                      float timeout = 0);

            /**
             * Recv packet from remote
             * @param size buffer size
             * @param addr remote address
             * @param port remote port
             * @result received length
             */
            std::size_t recv(void *data, std::size_t size,
                             std::string &addr, std::uint16_t &port,
                             float timeout = 0);
        };
    }
}