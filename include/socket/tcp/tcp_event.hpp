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
            enum class Type {Connecting = 1, Connected, Disconnect, Send, Recv};

            explicit event(Type t) : type(t) {}
            virtual ~event() = default;

            Type type;
        };


        // ---------------------------------------------------------------------
        // connecting_event
        struct connecting_event : public event
        {
            connecting_event() : event(Type::Connecting) {}

            net::endpoint ep;
        };


        // ---------------------------------------------------------------------
        // connected_event
        struct connected_event : public event
        {
            connected_event() : event(Type::Connected) {}

            net::endpoint ep;
            std::error_code err;
        };


        // ---------------------------------------------------------------------
        // disconnect_event
        struct disconnect_event : public event
        {
            disconnect_event() : event(Type::Disconnect) {}

            std::error_code err;
        };


        // ---------------------------------------------------------------------
        // send_event
        struct send_event : public event
        {
            send_event() : event(Type::Send) {}

            std::size_t size;
        };


        // ---------------------------------------------------------------------
        // recv_event
        struct recv_event : public event
        {
            recv_event() : event(Type::Recv) {}

            std::vector<std::uint8_t> data;
        };
    }
}