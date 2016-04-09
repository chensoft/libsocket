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
#include <mutex>

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
            virtual void start();
            virtual void start(const std::string &addr, std::uint16_t port);

            // todo shutdown immediately
            /**
             * Stop the server
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
            virtual void notify(std::vector<std::uint8_t> data,
                                std::string addr,
                                std::uint16_t port);

        public:
            /**
             * Server address
             */
            virtual std::string addr() const;

            /**
             * Server port
             */
            virtual std::uint16_t port() const;

        public:
            /**
             * Bind socket to specific port
             */
            virtual void bind(const std::string &addr, std::uint16_t port);

            /**
             * Close socket
             */
            virtual void close() override;

        protected:
            std::string _addr;
            std::uint16_t _port = 0;

            bool _quit = false;
            std::mutex _mutex;

            callback_type _callback;
        };
    }
}