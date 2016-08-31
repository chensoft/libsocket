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
#include <string>

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
std::error_code chen::net::notifier::add(socket_t fd, Type type, callback_type callback) noexcept
{
    return this->add(fd, type, false, callback);
}

std::error_code chen::net::notifier::add(socket_t fd, Type type, bool once, callback_type callback) noexcept
{
    struct kevent event{};
    int filter = 0;

    switch (type)
    {
        case Type::Read:
            filter = EVFILT_READ;
            EV_SET(&event, fd, EVFILT_READ, EV_ADD | (once ? EV_ONESHOT : 0), 0, 0, nullptr);
            break;

        case Type::Write:
            filter = EVFILT_WRITE;
            EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | (once ? EV_ONESHOT : 0), 0, 0, nullptr);
            break;
    }

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        return sys::error();

    this->_map[fd][filter] = callback;

    return {};
}

std::error_code chen::net::notifier::del(socket_t fd) noexcept
{
    this->del(fd, Type::Read);
    this->del(fd, Type::Write);
    this->_map.erase(fd);
    return {};
}

std::error_code chen::net::notifier::del(socket_t fd, Type type) noexcept
{
    struct kevent event{};
    int filter = 0;

    switch (type)
    {
        case Type::Read:
            filter = EVFILT_READ;
            EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
            break;

        case Type::Write:
            filter = EVFILT_WRITE;
            EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
            break;
    }

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        return sys::error();

    auto &item = this->_map[fd];
    item.erase(filter);

    return {};
}

// wait
std::error_code chen::net::notifier::wait() noexcept
{
    // todo add exit method
    struct kevent event{};

    while (true)
    {
        if (::kevent(this->_fd, nullptr, 0, &event, 1, nullptr) != 1)
            return sys::error();

        auto  sock = static_cast<socket_t>(event.ident);
        auto &item = this->_map[sock];
        auto  find = item.find(event.filter);

        if (find != item.end())
        {
            // be careful with callback
            if (event.flags & EV_ONESHOT)
            {
                auto cb = find->second;
                item.erase(static_cast<Type>(event.filter));
                cb(sock);
            }
            else
            {
                find->second(sock);
            }
        }
    }

    return {};
}

#endif