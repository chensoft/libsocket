/**
 * Created by Jian Chen
 * @since  2016.08.31
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/base/base_notifier.hpp>
#include <socket/base/base_error.hpp>
#include <chen/sys/sys.hpp>
#include <sys/event.h>

// -----------------------------------------------------------------------------
// helper
namespace
{
    std::int16_t filterToInt(chen::notifier::Filter filter)
    {
        switch (filter)
        {
            case chen::notifier::Filter::Read:
                return EVFILT_READ;

            case chen::notifier::Filter::Write:
                return EVFILT_WRITE;
        }

        return 0;
    }

    chen::notifier::Event filterToEvent(std::int16_t filter, std::uint16_t flags)
    {
        if (flags & EV_EOF)
            return chen::notifier::Event::End;

        switch (filter)
        {
            case EVFILT_READ:
                return chen::notifier::Event::Read;

            case EVFILT_WRITE:
                return chen::notifier::Event::Write;

            default:
                return static_cast<chen::notifier::Event>(0);
        }
    }
}


// -----------------------------------------------------------------------------
// notifier - kqueue
std::shared_ptr<chen::notifier> chen::notifier::create()
{
    return std::make_shared<chen::notifier>();
}

chen::notifier::notifier()
{
    this->_fd = ::kqueue();
    if (this->_fd < 0)
        throw notifier_error("notifier: " + sys::error().message());
}

chen::notifier::notifier(notifier &&o)
{
    *this = std::move(o);
}

chen::notifier& chen::notifier::operator=(notifier &&o)
{
    if (this == &o)
        return *this;

    this->_fd = o._fd;
    o._fd     = 0;

    return *this;
}

chen::notifier::~notifier()
{
    ::close(this->_fd);
}

// add/del
std::error_code chen::notifier::add(socket_t fd, Filter filter, std::uint16_t flag)
{
    struct kevent event = {};
    std::uint16_t codes = EV_ADD;

    if (flag & FlagOnce)
        codes |= EV_ONESHOT;

    if (flag & FlagEdge)
        codes |= EV_CLEAR;

    EV_SET(&event, fd, ::filterToInt(filter), codes, 0, 0, nullptr);
    return ::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::notifier::del(socket_t fd)
{
    if (!this->del(fd, Filter::Read))
        return sys::error();

    return this->del(fd, Filter::Write);
}

std::error_code chen::notifier::del(socket_t fd, Filter filter)
{
    struct kevent event{};
    EV_SET(&event, fd, ::filterToInt(filter), EV_DELETE, 0, 0, nullptr);
    return ::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0 ? sys::error() : std::error_code();
}

// loop
std::error_code chen::notifier::loop()
{
    // todo add exit method
    struct kevent event{};

    while (true)
    {
        if (::kevent(this->_fd, nullptr, 0, &event, 1, nullptr) != 1)
            return sys::error();

        this->notify(static_cast<socket_t>(event.ident), ::filterToEvent(event.filter, event.flags));
    }
}

// callback
void chen::notifier::attach(socket_t fd, callback_type callback)
{
    this->_map[fd] = callback;
}

void chen::notifier::detach(socket_t fd)
{
    this->del(fd);  // delete fd if user want to detach callback for it
    this->_map.erase(fd);
}

void chen::notifier::notify(socket_t fd, Event code)
{
    auto it = this->_map.find(fd);

    if (it != this->_map.end())
    {
        it->second(Data{
                .ev   = this,
                .fd   = fd,
                .code = code
        });
    }
}

#endif