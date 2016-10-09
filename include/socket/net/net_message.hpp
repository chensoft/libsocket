/**
 * Created by Jian Chen
 * @since  2016.10.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <vector>

namespace chen
{
    namespace net
    {
        class message
        {
        public:
            /**
             * Construct by desired size, usually used to read data
             */
            message(std::size_t size);

            /**
             * Construct by exist buffer, usually used to write data
             */
            message(std::vector<std::uint8_t> &&buffer);
            message(const std::vector<std::uint8_t> &buffer);

        public:
            /**
             * Original buffer size before send or recv
             */
            std::size_t origin() const;

            /**
             * Buffer object
             */
            std::vector<std::uint8_t>& buffer();
            const std::vector<std::uint8_t>& buffer() const;

        private:
            std::size_t _origin = 0;
            std::vector<std::uint8_t> _buffer;
        };
    }
}