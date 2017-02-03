/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>

namespace chen
{
    /**
     * This class represent a handle value, fd will be closed in destructor automatically
     */
    class basic_handle
    {
    public:
        basic_handle() = default;
        ~basic_handle() noexcept;

    public:
        /**
         * Native handle value
         */
        handle_t native() const noexcept
        {
            return this->_fd;
        }

        /**
         * Change handle value
         */
        void change(handle_t fd) noexcept;

        /**
         * Close the handle
         */
        void close() noexcept;

        /**
         * Transfer the handle to the outside, you must close the handle manually
         * @note this method is dangerous, you may leak the handle if you forget to close it
         */
        handle_t transfer() noexcept;

    private:
        /**
         * Disable copy & move, if you want to store object in container
         * you can use smart pointer like std::unique_ptr<basic_handle>
         */
        basic_handle(const basic_handle&) = delete;
        basic_handle& operator=(const basic_handle&) = delete;

    private:
        handle_t _fd = invalid_handle;
    };
}