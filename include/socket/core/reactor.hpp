/**
 * Created by Jian Chen
 * @since  2016.10.15
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

        /**
         * Event callback
         */
        typedef std::function<void (void *ptr, Event type)> callback_type;

    public:
        reactor() = default;

    public:
        /**
         * Set events and callback
         */
        void set(handle_t fd, void *ptr, int opcode, int flag, callback_type callback);

        /**
         * Remove events & callbacks
         * @attention call this method when the fd is destroyed
         */
        void del(handle_t fd);

    public:
        /**
         * Run the loop
         * when timeout is negative, it means wait forever, usually you can pass -1 to it
         * when timeout is zero, the poll method will return immediately, an event may or may not return
         * when timeout is positive, the time unit is second, e.g: 1.15 means 1.15 seconds to wait
         * @param count how many events you want to monitor per loop, it's just a hint, default is 1
         * @attention this method will not return unless user request to stop, timeout or interrupted
         */
        void run(std::size_t count = 1, double timeout = -1);

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

        std::size_t _count = 0;
        std::vector<backend::Data> _caching;
        std::unordered_map<handle_t, callback_type> _mapping;
    };
}