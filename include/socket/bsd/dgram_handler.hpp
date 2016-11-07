/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/dgram_client.hpp>

namespace chen
{
    class dgram_client_handler
    {
    public:
        virtual ~dgram_client_handler() = default;

    public:
        virtual void onRead(chen::dgram_client &c, std::vector<std::uint8_t> data, chen::basic_address addr) {}

        // todo pass original data to write
        virtual void onWrite(chen::dgram_client &c, std::size_t size, const chen::basic_address &addr) {}
    };
}