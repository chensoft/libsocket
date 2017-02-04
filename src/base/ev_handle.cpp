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
chen::ev_handle::~ev_handle()
{
    this->close();
}

// control
void chen::ev_handle::change(handle_t fd)
{
    this->close();
    this->_fd = fd;
}

void chen::ev_handle::close()
{
    if (this->_fd == invalid_handle)
        return;

#ifdef _WIN32
    // WSAPoll only support SOCKET, so use closesocket is correct
    ::closesocket(this->transfer());
#else
    ::close(this->transfer());
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