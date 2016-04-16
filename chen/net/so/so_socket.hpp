/**
 * Created by Jian Chen
 * @since  2016.04.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace chen
{
    namespace so
    {
        class socket
        {
        public:
            enum class Shutdown {Read, Write, Both};

        public:
            /**
             * Construct using domain, type and protocol
             */
            socket(int domain, int type, int protocol);

            /**
             * Construct with a new socket
             * @param so socket after accept, int on Unix, SOCKET on Windows
             */
            explicit socket(void *so, int domain, int type, int protocol);

            virtual ~socket() = 0;

        public:
            /**
             * Close socket
             */
            virtual void close();

            /**
             * Shutdown socket
             */
            virtual void shutdown(Shutdown flag = Shutdown::Both);

        public:
            /**
             * Remote server info
             */
            virtual std::string   remoteAddr() const;
            virtual std::uint16_t remotePort() const;

            /**
             * Local client info
             */
            virtual std::string   localAddr() const;
            virtual std::uint16_t localPort() const;

        protected:
            /**
             * Create socket
             */
            void build();

        private:
            socket(const socket&) = delete;
            socket& operator=(const socket&) = delete;

        protected:
            struct impl;
            std::unique_ptr<impl> _impl;
        };
    }
}