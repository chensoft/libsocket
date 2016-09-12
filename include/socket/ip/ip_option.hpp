/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/bsd_option.hpp>

namespace chen
{
    namespace ip
    {
        class option : public bsd::option
        {
        public:
            using bsd::option::option;

        public:
            /**
             * IP_TTL(IP packet's ttl value)
             */
            int ttl() const;
            bool ttl(int val);
        };
    }
}