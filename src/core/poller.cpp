/**
 * Created by Jian Chen
 * @since  2016.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/core/poller.hpp>
#include <chen/sys/sys.hpp>

 // -----------------------------------------------------------------------------
 // define
#ifdef _WIN32
namespace
{
	inline int poll(LPWSAPOLLFD fdArray, ULONG fds, INT timeout)
	{
		return ::WSAPoll(fdArray, fds, timeout);
	}
}
#else
#include <alloca.h>
#endif

// only Linux defines POLLRDHUP, you must
// use it to detect close event on Linux
#ifndef POLLRDHUP
#define POLLRDHUP 0
#endif


// -----------------------------------------------------------------------------
// poller
chen::poller::poller()
{
}

chen::poller::~poller()
{
}

// modify
void chen::poller::set(handle_t fd, int opcode, int flag)
{
    auto &item = this->_fds[fd];

    item.first.fd = fd;
    item.first.events = POLLRDHUP;

    if (opcode & OpcodeRead)
        item.first.events |= POLLIN;

    if (opcode & OpcodeWrite)
        item.first.events |= POLLOUT;

    item.second = flag;
}

void chen::poller::del(handle_t fd)
{
    this->_fds.erase(fd);
}

std::size_t chen::poller::poll(std::vector<Data> &cache, std::size_t count, double timeout)
{
    // ignore if it's empty or just a wakeup socket in it
    if (!count || this->_fds.empty() || (this->_fds.begin()->first == this->_wake.native()))
        return 0;

    // reset wake event
    if (!this->_wake)
    {
        this->_wake.reset(AF_INET, SOCK_DGRAM);
        this->set(this->_wake.native(), OpcodeRead);
    }

    // temporary use only
    auto scan = static_cast<::pollfd*>(alloca(sizeof(::pollfd) * this->_fds.size()));  // simulate VLA
    auto data = this->_fds;
    auto copy = 0;

    for (auto &pair : data)
        scan[copy++] = pair.second.first;

    // poll next events
    auto result = ::poll(scan, static_cast<unsigned>(data.size()), timeout < 0 ? -1 : static_cast<int>(timeout * 1000));

    if (result <= 0)
    {
        // EINTR maybe triggered by debugger, treat it as user request to stop
        if ((errno == EINTR) || !result)
            return 0;
        else
            throw std::system_error(sys::error(), "poller: failed to poll event");
    }

    // user request to stop
    if (!this->_wake)
        return 0;

    // collect poll data
    auto origin = cache.size();

    for (std::size_t i = 0, l = data.size(); i < l; ++i)
    {
        auto &event = scan[i];
        if (event.revents == 0)
            continue;

        // check events, multiple events maybe occur
        auto insert = [&] (Event code) {
            // remove fd if Ended event occurs or flag is once
            if ((code == Event::Ended) || (data[event.fd].second & FlagOnce))
                this->del(event.fd);

            if (i < origin)
                cache[i] = Data(event.fd, code);
            else
                cache.emplace_back(Data(event.fd, code));
        };

        if ((event.revents & POLLRDHUP) || (event.revents & POLLERR) || (event.revents & POLLHUP))
        {
            insert(Event::Ended);
        }
        else
        {
            if (event.revents & POLLIN)
                insert(Event::Readable);

            if (event.revents & POLLOUT)
                insert(Event::Writable);
        }
    }

    return static_cast<std::size_t>(cache.size() - origin);
}

std::vector<chen::poller::Data> chen::poller::poll(std::size_t count, double timeout)
{
    std::vector<chen::poller::Data> ret;
    this->poll(ret, count, timeout);
    return ret;
}

void chen::poller::stop()
{
    // notify wake message via socket's close event
    this->_wake.close();
}