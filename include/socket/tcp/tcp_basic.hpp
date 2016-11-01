/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_socket.hpp>
#include <socket/tcp/tcp_option.hpp>

namespace chen
{
    namespace tcp
    {
        class basic : public net::socket
        {
        public:
            explicit basic(net::runloop &runloop);

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
            using net::socket::reset;

            void reset(ip::address::Type family);
        };
    }
}