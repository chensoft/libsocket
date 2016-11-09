/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/base/bsd_socket.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// bsd_socket

// cleanup
void chen::bsd_socket::shutdown(Shutdown type) noexcept
{
    switch (type)
    {
        case Shutdown::Read:
            ::shutdown(this->_fd, SHUT_RD);
            break;

        case Shutdown::Write:
            ::shutdown(this->_fd, SHUT_WR);
            break;

        case Shutdown::Both:
            ::shutdown(this->_fd, SHUT_RDWR);
            break;
    }
}

void chen::bsd_socket::close() noexcept
{
    ::close(this->_fd);
    this->_fd = invalid_socket;
}

// property
std::error_code chen::bsd_socket::nonblocking(bool enable) noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return sys::error();

    return !::fcntl(this->_fd, F_SETFL, enable ? (flag | O_NONBLOCK) : (flag & ~O_NONBLOCK)) ? std::error_code() : sys::error();
}

std::size_t chen::bsd_socket::available() const noexcept
{
    std::size_t bytes = 0;
    return ::ioctl(this->_fd, FIONREAD, &bytes) < 0 ? 0 : bytes;
}

#endif