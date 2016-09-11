/**
 * Created by Jian Chen
 * @since  2016.08.31
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>
#include <system_error>
#include <functional>

namespace chen
{
    class socket;

    class notifier
    {
    public:
        enum class Filter {Read = 1, Write};
        enum class Event  {Read = 1, Write, End};  // End means disconnected or connection refused

        static constexpr std::uint16_t FlagOnce = 1 << 0;  // one shot
        static constexpr std::uint16_t FlagEdge = 1 << 1;  // edge triggered

        typedef std::function<void (chen::notifier &n, chen::notifier::Event ev)> callback_type;
        
    public:
        notifier();

        notifier(notifier &&o);
        notifier& operator=(notifier &&o);

        ~notifier();

    public:
        /**
         * Add and listen specific event
         * notifier will call socket's onEvent method if event happened
         * @param flag FlagOnce, FlagEdge...
         */
        std::error_code add(socket *ptr, Filter filter, std::uint16_t flag = 0);

        /**
         * Delete all events or specific event
         */
        std::error_code del(socket *ptr);
        std::error_code del(socket *ptr, Filter filter);

        /**
         * Wait events and dispatch
         */
        std::error_code loop();

    private:
        notifier(const notifier&) = delete;
        notifier& operator=(const notifier&) = delete;

    private:
        notifier_t _fd = notifier_t();
    };
}