/**
 * Created by Jian Chen
 * @since  2016.08.31
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>
#include <unordered_map>
#include <functional>
#include <memory>

namespace chen
{
    class notifier
    {
    public:
        enum class Filter {Read = 1, Write};
        enum class Event  {Read = 1, Write, End};  // End means disconnected or connection refused

        static constexpr std::uint16_t FlagOnce = 1 << 0;  // one shot
        static constexpr std::uint16_t FlagEdge = 1 << 1;  // edge triggered

        typedef struct
        {
            notifier *ev;
            socket_t  fd;
            Event   code;
        } Data;

        typedef std::function<void (chen::notifier::Data data)> callback_type;

    public:
        /**
         * Create shared notifier
         */
        static std::shared_ptr<notifier> create();

    public:
        notifier();

        notifier(notifier &&o);
        notifier& operator=(notifier &&o);

        ~notifier();

    public:
        /**
         * Add and listen specific event
         * @param flag FlagOnce, FlagEdge...
         */
        std::error_code add(socket_t fd, Filter filter, std::uint16_t flag = 0);

        /**
         * Delete all events or specific event
         */
        std::error_code del(socket_t fd);
        std::error_code del(socket_t fd, Filter filter);

        /**
         * Wait events and dispatch
         */
        std::error_code loop();

    public:
        /**
         * Set callback for the socket
         * @caution make sure the socket is valid until detached
         */
        void attach(socket_t fd, callback_type callback);

        /**
         * Remove callback of the socket
         * @caution all the events belong to this socket will be deleted
         */
        void detach(socket_t fd);

        /**
         * Internal use only, emit the event
         */
        void notify(socket_t fd, Event code);

    private:
        notifier(const notifier&) = delete;
        notifier& operator=(const notifier&) = delete;

    private:
        notifier_t _fd = notifier_t();
        std::unordered_map<socket_t, callback_type> _map;
    };
}