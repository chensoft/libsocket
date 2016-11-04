/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/inet/inet_socket.hpp>
#include <socket/net/tcp/tcp_option.hpp>

namespace chen
{
    namespace tcp
    {
        class basic : public inet_socket
        {
        public:
            /**
             * Get/Set socket option
             * Usage:
             * >> auto opt = socket.option();
             * >> opt.reuseaddr(true);           // set option
             * >> bool reuse = opt.reuseaddr();  // get option
             */
            tcp::option option();

        protected:
            /**
             * Reset socket
             */
            using inet_socket::reset;

            void reset(ip_address::Type family);
        };
    }
}