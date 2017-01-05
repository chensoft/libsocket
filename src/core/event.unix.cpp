/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/core/event.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// event
chen::event::event()
{
    if (::pipe(this->_pp) < 0)
        throw std::system_error(sys::error(), "event: failed to create pipe");

    if (::fcntl(this->_pp[0], F_SETFL, ::fcntl(this->_pp[0], F_GETFL, 0) | O_NONBLOCK) ||
        ::fcntl(this->_pp[1], F_SETFL, ::fcntl(this->_pp[1], F_GETFL, 0) | O_NONBLOCK))
    {
        ::close(this->_pp[0]);
        ::close(this->_pp[1]);
        throw std::system_error(sys::error(), "event: failed to create pipe");
    }
}

chen::event::~event()
{
    ::close(this->_pp[0]);
    ::close(this->_pp[1]);
}

void chen::event::set()
{
    ::write(this->_pp[1], "\n", 1);
}

void chen::event::reset()
{
    char buf[512];

    while (::read(this->_pp[0], buf, 512) >= 0)
        ;
}

chen::handle_t chen::event::native() const
{
    return this->_pp[0];
}

#endif