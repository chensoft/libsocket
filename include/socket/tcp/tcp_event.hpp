/**
 * Created by Jian Chen
 * @since  2016.09.04
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/base_endpoint.hpp>

namespace chen
{
    namespace tcp
    {
        namespace event
        {
            // -----------------------------------------------------------------
            // basic
            class basic
            {
            public:
                enum class Type {Connecting = 1, Connected, Disconnect, Read, Write};

            public:
                explicit basic(Type t) : type(t) {}

            public:
                Type type;
            };


            // -----------------------------------------------------------------
            // connecting(socket start connecting to remote)
            class connecting : public basic
            {
            public:
                connecting() : basic(Type::Connecting) {}

            public:
                endpoint ep = nullptr;
            };


            // -----------------------------------------------------------------
            // connected(connect success or failure will call this callback)
            class connected : public basic
            {
            public:
                connected() : basic(Type::Connected) {}

            public:
                endpoint ep = nullptr;
                std::error_code err;
            };


            // -----------------------------------------------------------------
            // disconnect(connection broken)
            class disconnect : public basic
            {
            public:
                disconnect() : basic(Type::Disconnect) {}

            public:
                std::error_code err;
            };


            // -----------------------------------------------------------------
            // read(received data from remote host)
            class read : public basic
            {
            public:
                read() : basic(Type::Read) {}

            public:
                std::vector<std::uint8_t> data;
            };


            // -----------------------------------------------------------------
            // write(send data to socket buffer)
            class write : public basic
            {
            public:
                write() : basic(Type::Write) {}

            public:
                std::size_t size;
            };
        }
    }
}