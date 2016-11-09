/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_socket.hpp>
#include <cstring>
#include <string>

namespace chen
{
    template <typename Address, typename Option>
    class stream_socket : public basic_socket<Address, Option>
    {
    public:
        /**
         * Read a packet from remote
         * @param size the desired read length, actual size will be less or equal than this value
         */
        ssize_t read(void *data, std::size_t size, int flags = 0) noexcept
        {
            return this->_socket.recv(data, size, flags);
        }

        /**
         * Write a packet to remote
         */
        ssize_t write(const char *text, int flags = 0) noexcept
        {
            return this->write(text, ::strlen(text), flags);
        }

        ssize_t write(const std::string &text, int flags = 0) noexcept
        {
            return this->write(text.data(), text.size(), flags);
        }

        ssize_t write(const void *data, std::size_t size, int flags = 0) noexcept
        {
            return this->_socket.send(data, size, flags);
        }
    };
}