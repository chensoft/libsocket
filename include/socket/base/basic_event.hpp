/**
 * Created by Jian Chen
 * @since  2017.01.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>
#include <functional>

namespace chen
{
    class reactor;

    /**
     * This is the event base class, other classes that want
     * to use in the reactor must be derived from this class
     */
    class basic_event
    {
    public:
        basic_event() = default;
        ~basic_event() noexcept;

    public:
        /**
         * Native handle value, reactor will use this value to register events
         */
        handle_t native() const noexcept
        {
            return this->_fd;
        }

        /**
         * Reset handle value, old fd will be removed from reactor
         * note that new fd will NOT add to reactor automatically
         */
        void reset(handle_t fd) noexcept;

        /**
         * Close the handle, fd will be removed from reactor
         */
        void close() noexcept;

        /**
         * Transfer the handle to the outside, you must close the handle manually
         * @note this method is dangerous, you may leak the handle if you forget to close it
         */
        handle_t transfer() noexcept;

    private:
        friend class reactor;

        /**
         * Used by reactor only, bind, delete and emit callback
         */
        void attach(reactor *rt, std::function<void (int type)> cb) noexcept;
        void detach() noexcept;
        void notify(int type) noexcept;

    private:
        /**
         * Disable copy
         * if you want to store event in container
         * use smart pointer like std::unique_ptr<basic_event>
         */
        basic_event(const basic_event&) = delete;
        basic_event& operator=(const basic_event&) = delete;

    private:
        handle_t _fd = invalid_handle;
        reactor *_rt = nullptr;
        std::function<void (int type)> _cb;
    };
}