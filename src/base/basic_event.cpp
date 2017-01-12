/**
 * Created by Jian Chen
 * @since  2017.01.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_event.hpp>
#include <socket/core/reactor.hpp>

// -----------------------------------------------------------------------------
// basic_event
chen::basic_event::~basic_event() noexcept
{
    this->close();
}

void chen::basic_event::change(handle_t fd) noexcept
{
    this->close();
    this->_fd = fd;
}

void chen::basic_event::close() noexcept
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

chen::handle_t chen::basic_event::transfer() noexcept
{
    if (this->_rt)
        this->_rt->del(this);

    auto temp = this->_fd;
    this->_fd = invalid_handle;
    return temp;
}

void chen::basic_event::attach(reactor *rt, std::function<void (int type)> cb) noexcept
{
    if (this->_rt)
        this->_rt->del(this);

    this->_rt = rt;
    this->_cb = cb;
}

void chen::basic_event::detach() noexcept
{
    this->_rt = nullptr;
    this->_cb = nullptr;
}

void chen::basic_event::notify(int type) noexcept
{
    if (this->_cb)
        this->_cb(type);
}