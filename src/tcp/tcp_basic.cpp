/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_basic.hpp>

// -----------------------------------------------------------------------------
// basic
chen::net::endpoint chen::tcp::basic::local() const
{
    return this->_socket.local();
}

chen::net::endpoint chen::tcp::basic::remote() const
{
    return this->_socket.remote();
}