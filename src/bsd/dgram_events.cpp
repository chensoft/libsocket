/**
 * Created by Jian Chen
 * @since  2016.11.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/dgram_events.hpp>

// -----------------------------------------------------------------------------
// dgram_read_event
chen::dgram_read_event::dgram_read_event(dgram_packet &&packet) : packet(std::move(packet))
{
}

chen::dgram_read_event::dgram_read_event(std::vector<std::uint8_t> &&data, const basic_address &addr)
        : packet(std::move(data), addr)
{
}

// -----------------------------------------------------------------------------
// dgram_write_event
chen::dgram_write_event::dgram_write_event(const dgram_packet &packet)
        : dgram_write_event(packet.data.data(), packet.data.size(), packet.addr)
{
}

chen::dgram_write_event::dgram_write_event(const std::uint8_t *data, std::size_t size, const basic_address &addr)
        : data(data), size(size), addr(addr)
{
}