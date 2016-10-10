/**
 * Created by Jian Chen
 * @since  2016.10.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_endpoint.hpp>

namespace chen
{
    namespace net
    {
        /**
         * This class is mainly used for net::proactor
         */
        class message
        {
        public:
            /**
             * Construct by desired size, usually used to read data
             */
            explicit message(std::size_t size);

            /**
             * Construct by exist buffer, usually used to write data
             */
            explicit message(std::vector<std::uint8_t> &&buffer);
            explicit message(const std::vector<std::uint8_t> &buffer);

            message(std::vector<std::uint8_t> &&buffer, const net::endpoint &remote);
            message(const std::vector<std::uint8_t> &buffer, const net::endpoint &remote);

        public:
            /**
             * Original buffer size before read or write
             */
            std::size_t origin() const;

            /**
             * Buffer object
             */
            std::vector<std::uint8_t>& buffer();
            const std::vector<std::uint8_t>& buffer() const;

            /**
             * Remote endpoint
             */
            net::endpoint& remote();
            const net::endpoint& remote() const;

        private:
            std::size_t _origin = 0;
            std::vector<std::uint8_t> _buffer;
            net::endpoint _remote;
        };
    }
}