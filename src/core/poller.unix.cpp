/**
 * Created by Jian Chen
 * @since  2016.11.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/core/poller.hpp>
#include <chen/sys/sys.hpp>
#include <unistd.h>

// -----------------------------------------------------------------------------
// only Linux define the POLLRDHUP, you must use it to detect close event on Linux
#ifndef POLLRDHUP
#define POLLRDHUP 0
#endif


// -----------------------------------------------------------------------------
// poller
chen::poller::poller()
{
    // create pipe to receive wake message
    if (::pipe(this->_pp) < 0)
        throw std::system_error(sys::error(), "poller: failed to create pipe");

    this->set(this->_pp[0], OpcodeRead);
}

chen::poller::~poller()
{
    ::close(this->_pp[0]);
    ::close(this->_pp[1]);
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

// poll
std::size_t chen::poller::poll(std::vector<Data> &cache, std::size_t count, double timeout)
{
    if (!count || (this->_fds.size() == 1))  // ignore if just has a pipe in it
        return 0;

    // temporary use only
    ::pollfd scan[this->_fds.size()];

    auto data = this->_fds;
    auto copy = 0;

    for (auto &pair : data)
        scan[copy++] = pair.second.first;

    // wait for events
    this->_wk = true;

    auto result = ::poll(scan, static_cast<nfds_t>(data.size()), timeout < 0 ? -1 : static_cast<int>(timeout * 1000));

    this->_wk = false;

    // check error status
    if (result <= 0)
    {
        // EINTR maybe triggered by debugger, treat it as user request to stop
        if ((errno == EINTR) || !result)
            return 0;
        else
            throw std::system_error(sys::error(), "poller: failed to poll event");
    }

    // check return data
    auto length = cache.size();

    for (std::size_t i = 0, l = data.size(); i < l; ++i)
    {
        auto &event = scan[i];
        if (event.revents == 0)
            continue;

        if (event.fd == this->_pp[0])
        {
            // user request to stop
            char dummy;
            ::read(this->_pp[0], &dummy, 1);
            return 0;
        }

        // check events, multiple events maybe occur
        auto insert = [&] (Event code) {
            // remove fd if Ended event occurs or flag is once
            if ((code == Event::Ended) || (data[event.fd].second & FlagOnce))
                this->del(event.fd);

            if (i < length)
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

    return (std::min)(static_cast<std::size_t>(result), count);
}

std::vector<chen::poller::Data> chen::poller::poll(std::size_t count, double timeout)
{
    std::vector<chen::poller::Data> ret;
    this->poll(ret, count, timeout);
    return ret;
}

void chen::poller::stop()
{
    if (!this->_wk)
        return;

    // notify wake message via pipe
    if (::write(this->_pp[1], "\n", 1) != 1)
        throw std::system_error(sys::error(), "poller: failed to wake the poller");
}

#endif