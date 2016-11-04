/**
 * Created by Jian Chen
 * @since  2016.09.04
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/prot/inet/net_endpoint.hpp>
#include <system_error>

namespace chen
{
    namespace tcp
    {
        // ---------------------------------------------------------------------
        // event
        class event
        {
        };


        // ---------------------------------------------------------------------
        // connected_event
        class connected_event : public event
        {
        public:
            connected_event(net::endpoint ep, std::error_code err) : ep(ep), err(err) {}

            net::endpoint ep;
            std::error_code err;
        };


        // ---------------------------------------------------------------------
        // disconnect_event
        class disconnect_event : public event
        {
        public:
            disconnect_event(std::error_code err) : err(err) {}

            std::error_code err;
        };


        // ---------------------------------------------------------------------
        // read_event
        class read_event : public event
        {
        public:
            read_event(std::vector<std::uint8_t> &&data) : data(std::move(data)) {}

            std::vector<std::uint8_t> data;
        };


        // ---------------------------------------------------------------------
        // write_event
        class write_event : public event
        {
        public:
            write_event(std::size_t size) : size(size) {}

            std::size_t size;
        };
    }
}