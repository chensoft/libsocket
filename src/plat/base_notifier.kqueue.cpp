/**
 * Created by Jian Chen
 * @since  2016.08.31
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

//#include <socket/base/base_notifier.hpp>
//#include <socket/base/base_error.hpp>
//#include <chen/sys/sys.hpp>
//#include <sys/event.h>
//
//// -----------------------------------------------------------------------------
//// helper
//namespace
//{
//    std::int16_t filterToInt(chen::notifier::Filter filter)
//    {
//        switch (filter)
//        {
//            case chen::notifier::Filter::Read:
//                return EVFILT_READ;
//
//            case chen::notifier::Filter::Write:
//                return EVFILT_WRITE;
//        }
//
//        return 0;
//    }
//
//    chen::notifier::Event filterToEvent(std::int16_t filter, std::uint16_t flags)
//    {
//        if (flags & EV_EOF)
//            return chen::notifier::Event::End;
//
//        switch (filter)
//        {
//            case EVFILT_READ:
//                return chen::notifier::Event::Read;
//
//            case EVFILT_WRITE:
//                return chen::notifier::Event::Write;
//
//            default:
//                return static_cast<chen::notifier::Event>(0);
//        }
//    }
//}
//
//
//// -----------------------------------------------------------------------------
//// notifier - kqueue
//chen::notifier::notifier()
//{
//    this->_fd = ::kqueue();
//    if (this->_fd < 0)
//        throw notifier_error("notifier: " + sys::error().message());
//}
//
//chen::notifier::notifier(notifier &&o)
//{
//    *this = std::move(o);
//}
//
//chen::notifier& chen::notifier::operator=(notifier &&o)
//{
//    if (this == &o)
//        return *this;
//
//    this->_fd = o._fd;
//    o._fd     = 0;
//
//    return *this;
//}
//
//chen::notifier::~notifier()
//{
//    ::close(this->_fd);
//}
//
//// add/del
//std::error_code chen::notifier::add(socket *ptr, Filter filter, std::uint16_t flag)
//{
//    struct kevent event = {};
//    std::uint16_t codes = EV_ADD;
//
//    if (flag & FlagOnce)
//        codes |= EV_ONESHOT;
//
//    if (flag & FlagEdge)
//        codes |= EV_CLEAR;
//
//    EV_SET(&event, ptr->native(), ::filterToInt(filter), codes, 0, 0, ptr);
//    return ::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0 ? sys::error() : std::error_code();
//}
//
//std::error_code chen::notifier::del(socket *ptr)
//{
//    if (!this->del(ptr, Filter::Read))
//        return sys::error();
//
//    return this->del(ptr, Filter::Write);
//}
//
//std::error_code chen::notifier::del(socket *ptr, Filter filter)
//{
//    struct kevent event{};
//    EV_SET(&event, ptr->native(), ::filterToInt(filter), EV_DELETE, 0, 0, ptr);
//    return ::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0 ? sys::error() : std::error_code();
//}
//
//// loop
//std::error_code chen::notifier::loop()
//{
//    // todo add exit method
//    struct kevent event{};
//
//    while (true)
//    {
//        if (::kevent(this->_fd, nullptr, 0, &event, 1, nullptr) != 1)
//            return sys::error();
//
//        auto ptr = static_cast<socket*>(event.udata);
//
//        if (ptr)
//            ptr->onEvent(*this, ::filterToEvent(event.filter, event.flags));
//        else
//            throw notifier_error("notifier: event happened but no related socket object");
//    }
//}

#endif