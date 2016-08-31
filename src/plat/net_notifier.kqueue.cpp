/**
 * Created by Jian Chen
 * @since  2016.08.31
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/net/net_notifier.hpp>
#include <socket/net/net_error.hpp>
#include <chen/sys/sys.hpp>
#include <sys/event.h>

// -----------------------------------------------------------------------------
// notifier - kqueue
chen::net::notifier& chen::net::notifier::standard()
{
    static notifier inst;
    return inst;
}

chen::net::notifier::notifier()
{
    this->_fd = ::kqueue();
    if (this->_fd < 0)
        throw error_notifier("notifier: " + sys::error().message());
}

chen::net::notifier::notifier(notifier &&o)
{
    *this = std::move(o);
}

chen::net::notifier& chen::net::notifier::operator=(notifier &&o)
{
    if (this == &o)
        return *this;

    this->_fd = o._fd;
    o._fd     = 0;

    return *this;
}

chen::net::notifier::~notifier()
{
    ::close(this->_fd);
}

// add/del
std::error_code chen::net::notifier::add(socket *ptr, Type type) noexcept
{
    struct kevent event{};

    switch (type)
    {
        case Type::Read:
            EV_SET(&event, ptr->native(), EVFILT_READ, EV_ADD, 0, 0, nullptr);
            break;

        case Type::Write:
            EV_SET(&event, ptr->native(), EVFILT_WRITE, EV_ADD, 0, 0, nullptr);
            break;
    }

    return ::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0 ? sys::error() : std::error_code();
    // todo record ptr
}

std::error_code chen::net::notifier::del(socket *ptr, Type type) noexcept
{
    struct kevent event{};

    switch (type)
    {
        case Type::Read:
            EV_SET(&event, ptr->native(), EVFILT_READ, EV_DELETE, 0, 0, nullptr);
            break;

        case Type::Write:
            EV_SET(&event, ptr->native(), EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
            break;
    }

    return ::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0 ? sys::error() : std::error_code();
    // todo record ptr
}

// wait
std::error_code chen::net::notifier::wait() noexcept
{
    // todo add exit method
    // todo add dispatch
    struct kevent event{};

    while (true)
    {
        if (::kevent(this->_fd, nullptr, 0, &event, 1, nullptr) == 1)
        {

        }
        else
        {
            return sys::error();
        }
    }

    return std::error_code();
}

#endif