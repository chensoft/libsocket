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

// property
bool chen::bsd::socket::nonblocking() const noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return false;

    return (flag & O_NONBLOCK) != 0;
}

bool chen::bsd::socket::nonblocking(bool enable) noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return false;

    return !::fcntl(this->_fd, F_SETFL, enable ? (flag | O_NONBLOCK) : (flag & ~O_NONBLOCK));
}

#endif