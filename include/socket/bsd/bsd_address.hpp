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
        class address
        {
        public:
            virtual ~address() = default;

        public:
            /**
             * Convert specific address to sockaddr_storage
             */
            virtual struct ::sockaddr_storage get() const = 0;

            /**
             * Set specific address via sockaddr_storage
             */
            virtual void set(const struct ::sockaddr_storage &val) = 0;

            /**
             * Current address's sizeof length
             */
            virtual socklen_t len() const = 0;
        };
    }
}