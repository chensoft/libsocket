/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "udp_error.h"

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// error
error::error(const std::string &what)
: std::runtime_error(what)
{

}


// -----------------------------------------------------------------------------
// error_socket
error_socket::error_socket(const std::string &what)
: error(what)
{

}


// -----------------------------------------------------------------------------
// error_bind
error_bind::error_bind(const std::string &what)
: error(what)
{

}


// -----------------------------------------------------------------------------
// error_send
error_send::error_send(const std::string &what)
: error(what)
{

}


// -----------------------------------------------------------------------------
// error_recv
error_recv::error_recv(const std::string &what)
: error(what)
{

}