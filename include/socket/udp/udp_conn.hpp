/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/base_common.hpp>

namespace chen
{
    namespace udp
    {
        class conn : public common
        {
        public:
            /**
             * Send data back to the host
             * todo make flags to enum class
             * todo add deadline
             */
            ssize_t send(const void *data, std::size_t size, int flags);
            ssize_t send(const std::vector<std::uint8_t> &data, int flags);

        public:
            /**
             * Remote's request data
             */
            std::vector<std::uint8_t> data() const;

            /**
             * Remote endpoint
             */
            endpoint remote() const;
        };
    }
}