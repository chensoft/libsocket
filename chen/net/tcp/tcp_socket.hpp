/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace chen
{
    namespace tcp
    {
        class socket
        {
        public:
            enum class Shutdown {Read, Write, Both};

        public:
            socket();
            virtual ~socket() = 0;

        public:
            /**
             * Send packet to remote
             */
            virtual void send(const std::uint8_t *data,
                              std::size_t size,
                              float timeout = 0);

            /**
             * Recv packet from remote
             */
            virtual void recv(std::uint8_t *data,
                              std::size_t &size,
                              float timeout = 0);

            /**
             * Close socket
             */
            virtual void close();

            /**
             * Shutdown socket
             */
            virtual void shutdown(Shutdown flag = Shutdown::Both);

        protected:
            /**
             * Build socket
             */
            virtual void build();

        private:
            socket(const socket&) = delete;
            socket& operator=(const socket&) = delete;

        protected:
            struct impl;
            std::unique_ptr<impl> _impl;
        };
    }
}