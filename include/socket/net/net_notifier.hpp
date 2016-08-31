/**
 * Created by Jian Chen
 * @since  2016.08.31
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_config.hpp>
#include <unordered_map>
#include <functional>

namespace chen
{
    namespace net
    {
        class notifier
        {
        public:
            enum Type {Read = 1, Write};

            typedef std::function<void (chen::net::socket_t fd)> callback_type;

        public:
            static notifier& standard();

        public:
            notifier();

            notifier(notifier &&o);
            notifier& operator=(notifier &&o);

            ~notifier();

        public:
            /**
             * Add and listen specific events
             * @caution make sure the socket is valid until deleted
             */
            std::error_code add(socket_t fd, Type type, callback_type callback) noexcept;
            std::error_code add(socket_t fd, Type type, bool once, callback_type callback) noexcept;

            /**
             * Delete all events or specific event
             */
            std::error_code del(socket_t fd) noexcept;
            std::error_code del(socket_t fd, Type type) noexcept;

            /**
             * Wait events and dispatch
             */
            std::error_code wait() noexcept;

        private:
            notifier(const notifier&) = delete;
            notifier& operator=(const notifier&) = delete;

        private:
            notifier_t _fd = notifier_t();
            std::unordered_map<socket_t, std::unordered_map<int, callback_type>> _map;
        };
    }
}