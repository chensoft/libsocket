/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_handle.hpp>

// -----------------------------------------------------------------------------
// basic_handle
chen::basic_handle::~basic_handle() noexcept
{
    this->close();
}

void chen::basic_handle::change(handle_t fd) noexcept
{
    this->close();
    this->_fd = fd;
}

void chen::basic_handle::close() noexcept
{
    auto fd = this->transfer();
    if (fd == invalid_handle)
        return;

#ifdef _WIN32
    // WSAPoll only support SOCKET, so use closesocket is correct
    ::closesocket(fd);
#else
    ::close(fd);
#endif
}

chen::handle_t chen::basic_handle::transfer() noexcept
{
    auto temp = this->_fd;
    this->_fd = invalid_handle;
    return temp;
}