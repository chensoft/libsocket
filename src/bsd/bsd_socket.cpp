/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/bsd_socket.hpp>

// -----------------------------------------------------------------------------
// socket
chen::bsd::socket::socket(socket_t fd) : _fd(fd)
{
}

chen::bsd::socket::socket(int domain, int type, int protocol) : _fd(::socket(domain, type, protocol))
{
}

chen::bsd::socket::socket(socket &&o)
{
    *this = std::move(o);
}

chen::bsd::socket& chen::bsd::socket::operator=(socket &&o)
{
    if (this == &o)
        return *this;

    this->_fd = o._fd;
    o._fd     = 0;

    return *this;
}

chen::bsd::socket::~socket()
{
    this->close();
}

// connection
std::error_code chen::bsd::socket::connect(const bsd::address &addr)
{
}

std::error_code chen::bsd::socket::bind(const bsd::address &addr)
{

}

std::error_code chen::bsd::socket::listen()
{

}

std::error_code chen::bsd::socket::listen(int backlog)
{

}

chen::socket_t chen::bsd::socket::accept()
{

}