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
    class reactor;

    /**
     * The base class used in conjunction with the reactor
     */
    class ev_base
    {
    public:
        /**
         * Event type
         * ---------------------------------------------------------------------
         * Readable: you can read data from remote
         * ---------------------------------------------------------------------
         * Writable: you can write data to remote
         * ---------------------------------------------------------------------
         * Closed: fd is closed, socket is disconnected or connection refused
         * ---------------------------------------------------------------------
         * @note in epoll, Closed event is always be monitored, in kqueue and poll
         * you must monitor the Readable event if you want to know the Closed event
         */
        static const int Readable;
        static const int Writable;
        static const int Closed;

    public:
        ev_base() = default;
        virtual ~ev_base() = default;

    public:
        /**
         * Properties
         */
        reactor* evLoop()
        {
            return this->_ev_loop;
        }

        int evMode() const
        {
            return this->_ev_mode;
        }

        int evFlag() const
        {
            return this->_ev_flag;
        }

        /**
         * Native handle value
         */
        virtual handle_t native() const = 0;

    protected:
        friend class reactor;

        /**
         * Notify that a reactor is attached or detached
         */
        virtual void onAttach(reactor *loop, int mode, int flag);
        virtual void onDetach();

        /**
         * Notify that at least one event has occurred
         * @note use bitwise AND to check the event type, e.g: if (type & Readable)
         */
        virtual void onEvent(int type) = 0;

    private:
        /**
         * Disable copy & move, if you want to store object in container
         * you can use smart pointer like std::unique_ptr<ev_base>
         */
        ev_base(const ev_base&) = delete;
        ev_base& operator=(const ev_base&) = delete;

    private:
        reactor *_ev_loop = nullptr;

        int _ev_mode = 0;
        int _ev_flag = 0;
    };
}