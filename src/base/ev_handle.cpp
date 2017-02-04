/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/ev_handle.hpp>
#include <socket/core/reactor.hpp>

// -----------------------------------------------------------------------------
// ev_handle
chen::ev_handle::ev_handle(std::function<void (int type)> cb) : _notify(cb)
{
}

chen::ev_handle::~ev_handle()
{
    this->close();
}

// notify
void chen::ev_handle::attach(std::function<void (int type)> cb)
{
    this->_notify = cb;
}

// control
void chen::ev_handle::change(handle_t fd)
{
    this->close();
    this->_fd = fd;
}

void chen::ev_handle::close()
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

chen::handle_t chen::ev_handle::transfer()
{
    if (this->evLoop())
        this->evLoop()->del(this);

    auto temp = this->_fd;
    this->_fd = invalid_handle;
    return temp;
}

// event
void chen::ev_handle::onAttach(reactor *loop, int mode, int flag)
{
    if (this->evLoop())
        this->evLoop()->del(this);

    ev_base::onAttach(loop, mode, flag);
}

void chen::ev_handle::onEvent(int type)
{
    auto loop = this->evLoop();
    auto func = this->_notify;

    if (loop && ((type & Closed) || (this->evFlag() & reactor::FlagOnce)))
        loop->del(this);

    if (func)
        func(type);
}