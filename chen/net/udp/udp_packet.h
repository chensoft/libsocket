/**
 * Created by Jian Chen
 * @since  2015.09.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace chen
{
    namespace udp
    {
        class packet
        {
        public:
            std::vector<std::uint8_t> data;
            std::size_t size = 0;

            std::string addr;
            std::uint16_t port = 0;
        };
    }
}