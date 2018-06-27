/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "socket/config.hpp"
#include <memory>

struct sockaddr_storage;

namespace chen
{
    /**
     * The base class for socket address
     */
    class basic_address
    {
    public:
        virtual ~basic_address() = default;

    public:
        /**
         * Underlying socket address length
         */
        virtual socklen_t socklen() const = 0;

        /**
         * Underlying socket address struct
         */
        virtual std::unique_ptr<struct ::sockaddr_storage> sockaddr() const = 0;
        virtual void sockaddr(const struct ::sockaddr *addr) = 0;
    };
}