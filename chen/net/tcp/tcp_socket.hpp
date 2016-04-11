/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/net/so/so_socket.hpp>

namespace chen
{
    namespace tcp
    {
        class socket : public chen::so::socket
        {
        public:
            socket();

        public:
            /**
             * Send packet to remote
             */
            virtual void send(const void *data, std::size_t size, float timeout = 0);

            /**
             * Recv packet from remote
             * @result received length
             */
            virtual std::size_t recv(void *data, std::size_t size, float timeout = 0);

        protected:
            /**
             * Build socket
             */
            virtual void build();
        };
    }
}