/**
 * Created by Jian Chen
 * @since  2016.11.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/dgram_packet.hpp>

namespace chen
{
    // -------------------------------------------------------------------------
    // dgram_read_event
    class dgram_read_event
    {
    public:
        dgram_read_event(dgram_packet &&packet);
        dgram_read_event(std::vector<std::uint8_t> &&data, const basic_address &addr);

    public:
        dgram_packet packet;
    };


    // -------------------------------------------------------------------------
    // dgram_write_event
    class dgram_write_event
    {
    public:
        dgram_write_event(const dgram_packet &packet);
        dgram_write_event(const std::uint8_t *data, std::size_t size, const basic_address &addr);

    public:
        const std::uint8_t *data;
        std::size_t size;

        const basic_address &addr;
    };
}