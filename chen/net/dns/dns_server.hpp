/**
 * Created by Jian Chen
 * @since  2015.12.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <functional>
#include <chen/net/udp/udp_server.hpp>

namespace chen
{
    namespace dns
    {
        class request;

        class server
        {
        public:
            /**
             * Callback type: request
             */
            typedef std::function<void (const chen::dns::request&)> callback_type;

        public:
            server();
            virtual ~server() = default;

        public:
            /**
             * Bind socket to specific port
             */
            virtual void bind(const std::string &addr, std::uint16_t port);

            /**
             * Start the server
             */
            virtual void start();

            /**
             * Shutdown the server
             */
            virtual void stop();

        public:
            /**
             * Set observer for server
             */
            virtual void attach(const callback_type &callback);

            /**
             * Set observer to empty
             */
            virtual void detach();

            /**
             * Notify the observer
             */
            virtual void notify(const chen::dns::request &request);

        public:
            /**
             * Server address
             */
            std::string addr() const;

            /**
             * Server port
             */
            std::uint16_t port() const;

        private:
            server(const server&) = delete;
            server& operator=(const server&) = delete;

        protected:
            /**
             * Receive server packet
             */
            virtual void onPacket(const std::vector<std::uint8_t> &data,
                                  const std::string &addr,
                                  std::uint16_t port);

        protected:
            chen::udp::server _udp;

            callback_type _callback;
        };
    }
}