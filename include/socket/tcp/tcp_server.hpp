/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_conn.hpp>
#include <functional>

namespace chen
{
    namespace tcp
    {
        // todo add notify
        class server : public basic
        {
        public:
            typedef std::function<void (chen::tcp::conn conn)> conn_callback;
            typedef std::function<void (chen::tcp::server *server, std::error_code code)> errc_callback;

        public:
            /**
             * Start the server
             */
            bool start(const endpoint &ep);
            bool start(const ip::address &addr, std::uint16_t port);

            /**
             * Stop the server
             */
            void stop();

        public:
            /**
             * Attach callbacks
             */
            void attach(conn_callback callback);
            void attach(errc_callback callback);

            void notify(chen::tcp::conn conn);
            void notify(chen::tcp::server *server, std::error_code code);

        private:
            bool _exit = false;

            conn_callback _conn_callback;
            errc_callback _errc_callback;
        };
    }
}