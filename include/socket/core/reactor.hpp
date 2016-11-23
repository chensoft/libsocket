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
#include <unordered_map>
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
         * Operation code type
         * @attention specific meaning can refer to backend
         */
        static constexpr int OpcodeRead  = backend::OpcodeRead;
        static constexpr int OpcodeWrite = backend::OpcodeWrite;
        static constexpr int OpcodeRW    = OpcodeRead | OpcodeWrite;

        /**
         * Specific flag type
         * @attention specific meaning can refer to backend
         */
        static constexpr int FlagOnce = backend::FlagOnce;
        static constexpr int FlagEdge = backend::FlagEdge;

        /**
         * Reactor event type
         * @attention specific meaning can refer to backend
         */
        using Event = backend::Event;

        typedef std::function<void (Event type)> callback_type;

    public:
        reactor() = default;

    public:
        /**
         * Set events and callback, if Ended event occurs then fd will be removed
         */
        void set(handle_t fd, int opcode, callback_type callback);
        void set(handle_t fd, int opcode, int flag, callback_type callback);

        /**
         * Remove events & callbacks
         * @attention socket should call this method when it is destroyed
         */
        void del(handle_t fd);

    public:
        /**
         * Run the loop
         * @param count max events count when use rector::fetch, default is 1
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