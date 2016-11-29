/**
 * Created by Jian Chen
 * @since  2016.11.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/service_kqueue.hpp>
#include <socket/base/service_poller.hpp>
#include <socket/base/service_epoll.hpp>
#include <functional>

namespace chen
{
    // -------------------------------------------------------------------------
    // reactor_delegate
    class reactor_delegate
    {
    public:
        virtual ~reactor_delegate() = default;

    public:
        virtual void onReadable() {}
        virtual void onWritable() {}
        virtual void onEnded() {}
    };


    // -------------------------------------------------------------------------
    // reactor
    class reactor
    {
    public:
        /**
         * Backend define
         */
#if !defined(__linux__) && !defined(_WIN32)
        typedef service_kqueue backend;
#elif defined(__linux__)
        typedef service_epoll backend;
#else
        typedef service_poller backend;
#endif

        /**
         * Operation code
         * @attention specific meaning can refer to backend class
         */
        static constexpr int OpcodeRead  = backend::OpcodeRead;
        static constexpr int OpcodeWrite = backend::OpcodeWrite;
        static constexpr int OpcodeRW    = backend::OpcodeRW;

        /**
         * Event flag
         * @attention specific meaning can refer to backend class
         */
        static constexpr int FlagOnce = backend::FlagOnce;
        static constexpr int FlagEdge = backend::FlagEdge;

        /**
         * Event type
         * @attention specific meaning can refer to backend class
         */
        using Event = backend::Event;

    public:
        reactor();
        ~reactor();

    public:
        /**
         * Set events and callback
         */
        void set(handle_t fd, reactor_delegate *delegate, int opcode, int flag);

        /**
         * Remove events & callbacks
         * @attention call this method when fd is destroyed
         */
        void del(handle_t fd);

    public:
        /**
         * Run the loop
         * @param count how many events you want to monitor per loop, it's just a hint, default is 1
         * @attention this method will not return unless user request to stop, timeout or interrupted
         */
        void run();
        void run(std::size_t count);

        /**
         * Stop the loop
         * @attention you can call this method in callback or other thread
         */
        void stop();

    private:
        reactor(const reactor&) = delete;
        reactor& operator=(const reactor&) = delete;

    private:
        backend _backend;
    };
}