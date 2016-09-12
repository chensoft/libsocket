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
        class address : public ::sockaddr_storage
        {
        public:
            address();

        public:
            bool operator==(const address &o) const;
            bool operator!=(const address &o) const;

            bool operator<(const address &o) const;
            bool operator>(const address &o) const;

            bool operator<=(const address &o) const;
            bool operator>=(const address &o) const;
        };
    }
}