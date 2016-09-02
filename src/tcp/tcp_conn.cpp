/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_conn.hpp>

// -----------------------------------------------------------------------------
// conn
chen::tcp::conn::conn(socket socket)
{
    this->_socket = std::move(socket);
}