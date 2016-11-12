/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

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
            ::shutdown(this->_fd, SD_RECEIVE);
            break;

        case Shutdown::Write:
            ::shutdown(this->_fd, SD_SEND);
            break;

        case Shutdown::Both:
            ::shutdown(this->_fd, SD_BOTH);
            break;
    }
}

void chen::basic_socket::close() noexcept
{
    ::closesocket(this->_fd);
    this->_fd = invalid_socket;
}

// property
std::error_code chen::basic_socket::nonblocking(bool enable) noexcept
{
    u_long mode = enable ? 1 : 0;
    return ::ioctlsocket(this->_fd, FIONBIO, &mode) ? sys::error() : std::error_code();
}

std::size_t chen::basic_socket::available() const noexcept
{
    std::size_t bytes = 0;
    return ::ioctlsocket(this->_fd, FIONREAD, reinterpret_cast<u_long*>(&bytes)) < 0 ? 0 : bytes;
}

#endif