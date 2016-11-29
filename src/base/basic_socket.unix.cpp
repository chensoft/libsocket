/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/base/basic_socket.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// basic_socket

// cleanup
void chen::basic_socket::shutdown(Shutdown type) noexcept
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

void chen::basic_socket::close() noexcept
{
    ::close(this->_fd);
    this->_fd = invalid_handle;
}

chen::handle_t chen::basic_socket::detach() noexcept
{
    auto temp = this->_fd;
    this->_fd = invalid_handle;
    return temp;
}

// property
std::error_code chen::basic_socket::nonblocking(bool enable) noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    return (flag >= 0) && !::fcntl(this->_fd, F_SETFL, enable ? (flag | O_NONBLOCK) : (flag & ~O_NONBLOCK)) ? std::error_code() : sys::error();
}

std::size_t chen::basic_socket::available() const noexcept
{
    int len = 0;
    int ret = ::ioctl(this->_fd, FIONREAD, &len);
    return static_cast<std::size_t>(ret < 0 ? 0 : len);
}

#endif