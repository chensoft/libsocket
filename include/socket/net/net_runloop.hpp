/**
 * Created by Jian Chen
 * @since  2016.10.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/bsd_poller.hpp>
#include <socket/bsd/bsd_kqueue.hpp>
#include <socket/bsd/bsd_epoll.hpp>
#include <socket/config.hpp>
#include <unordered_map>
#include <functional>

namespace chen
{
    namespace net
    {
        /**
         * An event runloop implement using bsd::reactor
         */
        class runloop
        {
        public:
            /**
             * Operation code type
             * @attention specific meaning can refer to bsd::reactor
             */
            static constexpr int OpcodeRead  = bsd::reactor::OpcodeRead;
            static constexpr int OpcodeWrite = bsd::reactor::OpcodeWrite;

            /**
             * Specific flag type
             * @attention specific meaning can refer to bsd::reactor
             */
            static const int FlagOnce;
            static const int FlagEdge;

            /**
             * Reactor event type
             * @attention specific meaning can refer to bsd::reactor
             */
            using Event = bsd::reactor::Event;

            typedef std::function<void (Event type)> callback_type;

        public:
            runloop() = default;

        public:
            /**
             * Set events and callback
             */
            void set(socket_t fd, int opcode, callback_type callback);
            void set(socket_t fd, int opcode, int flag, callback_type callback);

            /**
             * Remove events & callbacks
             * @attention socket should call this method when it is destroyed
             */
            void del(socket_t fd);

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
            runloop(const runloop&) = delete;
            runloop& operator=(const runloop&) = delete;

        private:
            bsd::reactor _reactor;

            std::size_t _count = 0;
            std::vector<bsd::reactor::Data> _caching;
            std::unordered_map<socket_t, callback_type> _mapping;
        };
    }
}