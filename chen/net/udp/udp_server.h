/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "udp_socket.h"
#include <functional>
#include <vector>

namespace chen
{
    namespace udp
    {
        class server : public socket
        {
        public:
            /**
             * Callback type: data, addr, port
             */
            typedef std::function<void (std::vector<std::uint8_t>,
                                        std::string,
                                        std::uint16_t)> callback_type;

        public:
            server();

        public:
            /**
             * Bind socket to specific port
             */
            virtual void bind(const std::string &addr, std::uint16_t port);

            /**
             * Start the server
             */
            virtual void start();

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
            virtual void notify(std::vector<std::uint8_t> data,
                                std::string addr,
                                std::uint16_t port);

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

        private:
            std::string _addr;
            std::uint16_t _port = 0;

            callback_type _callback;
        };
    }
}