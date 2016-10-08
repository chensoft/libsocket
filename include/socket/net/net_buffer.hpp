/**
 * Created by Jian Chen
 * @since  2016.10.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstdint>
#include <vector>

namespace chen
{
    namespace net
    {
        class buffer
        {
        public:
            /**
             * Init by desired size
             */
            buffer(std::size_t size);

            /**
             * Init by exist buffer
             */
            buffer(std::vector<std::uint8_t> &&data);
            buffer(const std::vector<std::uint8_t> &data);

        public:
            /**
             * Original buffer size before send or recv
             */
            std::size_t orig() const;
            void orig(std::size_t size);

            /**
             * Final buffer size after send or recv
             */
            std::size_t size() const;

            /**
             * Buffer object
             */
            std::vector<std::uint8_t>& data();
            const std::vector<std::uint8_t>& data() const;

        private:
            std::size_t _orig = 0;
            std::vector<std::uint8_t> _data;
        };
    }
}