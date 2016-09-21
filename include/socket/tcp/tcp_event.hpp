/**
 * Created by Jian Chen
 * @since  2016.09.04
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_endpoint.hpp>
#include <system_error>

namespace chen
{
    namespace tcp
    {
        // ---------------------------------------------------------------------
        // event
        struct event
        {
        };


        // ---------------------------------------------------------------------
        // connecting_event
        struct connecting_event : public event
        {
            connecting_event(net::endpoint ep) : ep(ep) {}

            net::endpoint ep;
        };


        // ---------------------------------------------------------------------
        // connected_event
        struct connected_event : public event
        {
            connected_event(net::endpoint ep, std::error_code err) : ep(ep), err(err) {}

            net::endpoint ep;
            std::error_code err;
        };


        // ---------------------------------------------------------------------
        // disconnect_event
        struct disconnect_event : public event
        {
            disconnect_event(std::error_code err) : err(err) {}

            std::error_code err;
        };


        // ---------------------------------------------------------------------
        // send_event
        struct send_event : public event
        {
            send_event(std::size_t size) : size(size) {}

            std::size_t size;
        };


        // ---------------------------------------------------------------------
        // recv_event
        struct recv_event : public event
        {
            recv_event(std::vector<std::uint8_t> &&data) : data(std::move(data)) {}

            std::vector<std::uint8_t> data;
        };
    }
}