/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/bsd/bsd_kqueue.hpp>
#include <chen/sys/sys.hpp>
#include <sys/event.h>
#include <unistd.h>

// -----------------------------------------------------------------------------
// kqueue
chen::bsd::kqueue::kqueue()
{
    if ((this->_fd = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to create kqueue");

    if (::pipe(this->_pp) < 0)
    {
        ::close(this->_fd);
        throw std::system_error(sys::error(), "kqueue: failed to create pipe");
    }

    // register pipe to receive exit message
    this->add(this->_pp[0], Opcode::Read);
}

chen::bsd::kqueue::~kqueue()
{
    ::close(this->_fd);
    ::close(this->_pp[0]);
    ::close(this->_pp[1]);
}

// modify
void chen::bsd::kqueue::add(int fd, Opcode opcode, std::uint16_t flag)
{
    struct ::kevent event{};
    std::uint16_t codes = EV_ADD;

    if (flag & FlagOnce)
        codes |= EV_ONESHOT;

    if (flag & FlagEdge)
        codes |= EV_CLEAR;

    EV_SET(&event, fd, this->opcode(opcode), codes, 0, 0, nullptr);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        throw std::system_error(chen::sys::error(), "kqueue: failed to add event");
}

void chen::bsd::kqueue::del(int fd)
{
    struct ::kevent event{};
    EV_SET(&event, fd, this->opcode(Opcode::Read) | this->opcode(Opcode::Write), EV_DELETE, 0, 0, nullptr);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");
}

// poll
chen::bsd::kqueue::Data chen::bsd::kqueue::poll()
{
    struct ::kevent event{};

    // poll next event
    if (::kevent(this->_fd, nullptr, 0, &event, 1, nullptr) != 1)
        throw std::system_error(sys::error(), "kqueue: failed to poll event");

    // check exit status
    if (event.ident == static_cast<uintptr_t>(this->_pp[0]))
    {
        char dummy;
        ::read(this->_pp[0], &dummy, 1);
        return {};
    }

    // return the data
    Data ret;

    ret.fd = static_cast<int>(event.ident);
    ret.ev = this->event(event.filter, event.flags);

    return ret;
}

void chen::bsd::kqueue::exit()
{
    // notify exit message via pipe
    if (::write(this->_pp[1], "\n", 1) != 1)
        throw std::system_error(sys::error(), "kqueue: failed to exit the kqueue");
}

// misc
std::int16_t chen::bsd::kqueue::opcode(Opcode opcode)
{
    switch (opcode)
    {
        case Opcode::Read:
            return EVFILT_READ;

        case Opcode::Write:
            return EVFILT_WRITE;
    }

    return 0;
}

chen::bsd::kqueue::Event chen::bsd::kqueue::event(std::int16_t opcode, std::uint16_t flags)
{
    if (flags & EV_EOF)
        return Event::End;

    switch (opcode)
    {
        case EVFILT_READ:
            return Event::Read;

        case EVFILT_WRITE:
            return Event::Write;

        default:
            throw std::runtime_error("kqueue: unknown event detect");
    }
}

#endif