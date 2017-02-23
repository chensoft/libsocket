/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/ev_base.hpp>
#include <socket/config.hpp>

namespace chen
{
    /**
     * The base class that has a file descriptor
     */
    class ev_handle: public ev_base
    {
    public:
        ~ev_handle();

    public:
        /**
         * Native handle value
         */
        virtual handle_t native() const
        {
            return this->_fd;
        }

    public:
        /**
         * Change handle value, old fd will be removed from reactor
         * note that new fd will NOT add to reactor automatically
         */
        virtual void change(handle_t fd);

        /**
         * Close the handle, fd will be removed from reactor
         */
        virtual void close();

        /**
         * Transfer the handle to the outside, you must close the handle manually
         * @note this method is dangerous, you may leak the handle if you forget to close it
         */
        virtual handle_t transfer();

    protected:
        friend class reactor;

        /**
         * Notify that a reactor is attached or detached
         */
        virtual void onAttach(reactor *loop, int mode, int flag);

        /**
         * Notify that at least one event has occurred
         */
        virtual void onEvent(int type) = 0;

    private:
        handle_t _fd = invalid_handle;
    };
}