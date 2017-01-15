/**
 * Created by Jian Chen
 * @since  2017.01.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_handle.hpp>
#include <socket/core/reactor.hpp>

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
    if (this->_rt)
        this->_rt->del(this);

    auto temp = this->_fd;
    this->_fd = invalid_handle;
    return temp;
}

void chen::basic_handle::attach(reactor *rt, std::function<void (int type)> cb, int mode, int flag) noexcept
{
    if (this->_rt)
        this->_rt->del(this);

    this->_rt = rt;
    this->_cb = cb;

    this->_mode = mode;
    this->_flag = flag;
}

void chen::basic_handle::detach() noexcept
{
    this->_rt = nullptr;
    this->_cb = nullptr;

    this->_mode = 0;
    this->_flag = 0;
}

void chen::basic_handle::notify(int type) noexcept
{
    if (this->_cb)
    {
        auto copy = this->_cb;
        copy(type);
    }
}

int chen::basic_handle::mode() const noexcept
{
    return this->_mode;
}

int chen::basic_handle::flag() const noexcept
{
    return this->_flag;
}