/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "udp_socket.hpp"
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
             * Receive Callback
             */
            typedef std::function<void (std::vector<std::uint8_t> data,
                                        std::string addr,
                                        std::uint16_t port)> callback_type;

        public:
            /**
             * Start the server
             */
            void start();
            void start(const std::string &addr, std::uint16_t port);

            /**
             * Stop the server
             */
            void stop();

        public:
            /**
             * Set observer for server
             */
            void attach(callback_type callback);

            /**
             * Set observer to empty
             */
            void detach();

            /**
             * Notify the observer
             */
            void notify(std::vector<std::uint8_t> data,
                        std::string addr,
                        std::uint16_t port);

        public:
            /**
             * Bind socket to specific port
             * use `start` method is more convenient
             */
            void bind(const std::string &addr, std::uint16_t port);

        protected:
            callback_type _callback;
        };
    }
}