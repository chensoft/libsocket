/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>

namespace chen
{
    namespace bsd
    {
        class endpoint
        {
        public:
            virtual ~endpoint() = default;

        public:
            /**
             * Convert specific endpoint to sockaddr_storage
             */
            virtual struct ::sockaddr_storage get() const = 0;

            /**
             * Set specific endpoint via sockaddr_storage
             */
            virtual void set(const struct ::sockaddr_storage &val) = 0;

            /**
             * Current endpoint's sizeof length
             */
            virtual socklen_t len() const = 0;
        };
    }
}