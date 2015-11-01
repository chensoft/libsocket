/**
 * Created by Jian Chen
 * @since  2015.11.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <map>
#include <string>
#include <cstdint>
#include <functional>
#include <chen/net/udp/udp_server.h>
#include "dns_packet.h"

namespace chen
{
    namespace dns
    {
        class server
        {
        public:
            typedef std::function<chen::dns::response (const chen::dns::request&)> handler_type;

        public:
            server();
            virtual ~server() = default;

        public:
            /**
             * Insert a zone
             */
            virtual void insert(const std::string &zone, handler_type handler);

            /**
             * Remove a zone
             */
            virtual void remove(const std::string &zone);

        public:
            /**
             * Listen & Serve dns service
             */
            virtual void run(const std::string &addr = "0.0.0.0", std::uint16_t port = 53);

        protected:
            /**
             * Callback for udp server
             */
            virtual void callback(chen::udp::socket *so, const chen::udp::packet &pkt);

        protected:
            chen::udp::server _udp;
            std::map<std::string, handler_type> _cache;
        };
    }
}