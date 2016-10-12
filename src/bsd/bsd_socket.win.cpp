/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/bsd/bsd_socket.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// socket

// cleanup
void chen::bsd::socket::shutdown(Shutdown type) noexcept
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

void chen::bsd::socket::close() noexcept
{
    if (!this->valid())
        return;

    ::closesocket(this->_fd);
    this->_fd = invalid_socket;
}

// property
std::error_code chen::bsd::socket::nonblocking(bool enable) noexcept
{
    u_long mode = enable ? 1 : 0;
    return ::ioctlsocket(this->_fd, FIONBIO, &mode) ? sys::error() : std::error_code();
}

#endif