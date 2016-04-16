/**
 * Created by Jian Chen
 * @since  2016.04.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "so_socket_unix.hpp"
#include <chen/net/tcp/tcp_conn.hpp>

using namespace chen;
using namespace chen::so;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// conn
conn::conn(void *val)
{
    this->_impl->_socket = *static_cast<int*>(val);
}

#endif