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
     * This class represent a handle value, classes that want
     * to use in the reactor can make an instance of this class
     */
    class basic_handle
    {
    public:
        basic_handle() = default;
        ~basic_handle() noexcept;

    public:
        /**
         * Native handle value, reactor will use this value to register events
         */
        operator chen::handle_t() const noexcept
        {
            return this->_fd;
        }

    public:
        /**
         * Change handle value, old fd will be removed from reactor
         * note that new fd will NOT add to reactor automatically
         */
        void change(handle_t fd) noexcept;

        /**
         * Close the handle, fd will be removed from reactor
         */
        void close() noexcept;

        /**
         * Transfer the handle to the outside, you must close the handle manually
         * @note this method is dangerous, you may leak the handle if you forget to close it
         */
        handle_t transfer() noexcept;

    public:
        /**
         * Used by reactor only, bind, delete and emit callback
         */
        void attach(reactor *rt, std::function<void (int type)> cb) noexcept;
        void detach() noexcept;
        void notify(int type) noexcept;

    private:
        /**
         * Disable copy & move
         * if you want to store object in container, you can
         * use smart pointer like std::unique_ptr<basic_handle>
         */
        basic_handle(const basic_handle&) = delete;
        basic_handle& operator=(const basic_handle&) = delete;

    private:
        handle_t _fd = invalid_handle;
        reactor *_rt = nullptr;
        std::function<void (int type)> _cb;
    };
}