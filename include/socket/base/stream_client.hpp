/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/stream_socket.hpp>

namespace chen
{
    template <typename Address, typename Option>
    class stream_client : public stream_socket<Address, Option>
    {
    public:
        /**
         * Connect to remote address
         */
        std::error_code connect(const Address &addr, bool nonblocking = false)
        {
            if (this->_socket)
                this->disconnect();

            this->_socket.reset(addr.addr().type());
            this->nonblocking(nonblocking);

            this->_remote = addr;
            return this->_socket.connect(addr);
        }

        /**
         * Disconnect and reconnect to remote
         */
        std::error_code reconnect()
        {
            return this->connect(this->_remote);
        }

        /**
         * Close the connection and clear the buffer
         */
        void disconnect() noexcept
        {
            this->shutdown();
            this->close();
        }

    protected:
        Address _remote;
    };
}