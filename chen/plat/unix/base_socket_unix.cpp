/**
 * Created by Jian Chen
 * @since  2016.04.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "base_socket_unix.hpp"
#include <chen/net/so/so_error.hpp>
#include <sys/socket.h>
#include <unistd.h>

using namespace chen;
using namespace chen::so;

// -----------------------------------------------------------------------------
// socket
socket::socket()
{
}

socket::~socket()
{
    if (this->_impl)
        this->close();
}

void socket::close()
{
    if (!this->_impl)
        return;

    ::close(this->_impl->_socket);
    this->_impl.reset();
}

void socket::shutdown(Shutdown flag)
{
    if (!this->_impl)
        return;

    if (flag == Shutdown::Read)
        ::shutdown(this->_impl->_socket, SHUT_RD);
    else if (flag == Shutdown::Write)
        ::shutdown(this->_impl->_socket, SHUT_WR);
    else if (flag == Shutdown::Both)
        ::shutdown(this->_impl->_socket, SHUT_RDWR);
}

#endif  // CHEN_OS_UNIX