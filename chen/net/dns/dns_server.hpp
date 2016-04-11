/**
 * Created by Jian Chen
 * @since  2015.12.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <functional>
#include <chen/net/udp/udp_server.hpp>
#include "dns_packet.hpp"

// todo re-design
namespace chen
{
    namespace dns
    {
        class request;

        class server
        {
        public:
            /**
             * DNS Callback
             */
            typedef std::function<void (chen::dns::request request)> callback_type;

        public:
            server();
            virtual ~server() = default;

        public:
            /**
             * Start the server
             */
            virtual void start();
            virtual void start(const std::string &addr, std::uint16_t port);

            /**
             * Shutdown the server
             */
            virtual void stop();

        public:
            /**
             * Set observer for server
             */
            virtual void attach(callback_type callback);

            /**
             * Set observer to empty
             */
            virtual void detach();

            /**
             * Notify the observer
             */
            virtual void notify(chen::dns::request request);

        public:
            /**
             * Bind socket to specific port
             */
            virtual void bind(const std::string &addr, std::uint16_t port);

        protected:
            /**
             * Receive server packet
             */
            virtual void onPacket(std::vector<std::uint8_t> data,
                                  std::string addr,
                                  std::uint16_t port);

        private:
            server(const server&) = delete;
            server& operator=(const server&) = delete;

        protected:
            // todo add tcp server
            chen::udp::server _udp;

            callback_type _callback;
        };
    }
}