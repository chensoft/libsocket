/**
 * Created by Jian Chen
 * @since  2016.11.07
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
    class dgram_socket : public basic_socket<Address, Option>
    {
    public:
        /**
         * Read a packet from remote
         * @param size the desired read length, actual size will be less or equal than this value
         */
        ssize_t read(void *data, std::size_t size, Address &addr, int flags = 0) noexcept
        {
            // todo let Address inherit from bsd_address?
            bsd_address tmp;
            auto ret = this->_socket.recvfrom(data, size, tmp, flags);

            addr = tmp;
            return ret;
        }

        /**
         * Write a packet to remote
         */
        ssize_t write(const char *text, const Address &addr, int flags = 0) noexcept
        {
            return this->write(text, ::strlen(text), addr, flags);
        }

        ssize_t write(const std::string &text, const Address &addr, int flags = 0) noexcept
        {
            return this->write(text.data(), text.size(), addr, flags);
        }

        ssize_t write(const void *data, std::size_t size, const Address &addr, int flags = 0) noexcept
        {
            return this->_socket.sendto(data, size, addr, flags);
        }
    };
}