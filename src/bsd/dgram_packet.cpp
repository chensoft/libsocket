/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/dgram_packet.hpp>

// -----------------------------------------------------------------------------
// dgram_packet
chen::dgram_packet::dgram_packet(std::vector<std::uint8_t> data, const basic_address &addr) : data(data), addr(addr)
{
}