/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/dgram_socket.hpp>

namespace chen
{
    template <typename T>
    class dgram_server : public dgram_socket<T>
    {
    public:
        typedef typename dgram_socket<T>::address_type address_type;

    public:
        /**
         * Construct according to address family
         */
        dgram_server(const address_type &addr, int protocol = 0)
        {
            this->_local = addr;
            this->_socket.reset(this->_local.addr.ss_family, SOCK_DGRAM, protocol);
        }

    public:
        /**
         * Bind and Listen on the port
         */
        std::error_code listen(int backlog = 0) noexcept
        {
            auto err = this->_socket.bind(this->_local);
            if (err)
                return err;

            return this->_socket.listen(backlog);
        }

    protected:
        bsd_address _local;
    };
}