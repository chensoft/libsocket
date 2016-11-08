/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/basic_address.hpp>
#include <cstdint>
#include <vector>

namespace chen
{
    class dgram_packet
    {
    public:
        dgram_packet() = default;
        dgram_packet(std::vector<std::uint8_t> &&data, const basic_address &addr);

    public:
        std::vector<std::uint8_t> data;
        basic_address addr;
    };
}