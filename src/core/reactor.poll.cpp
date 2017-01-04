/**
 * Created by Jian Chen
 * @since  2017.01.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/inet/inet_address.hpp>
#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>
#include <algorithm>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

const int chen::reactor::FlagEdge = 0;
const int chen::reactor::FlagOnce = 1;

const chen::reactor::Type chen::reactor::Readable = 1 << 0;
const chen::reactor::Type chen::reactor::Writable = 1 << 1;
const chen::reactor::Type chen::reactor::Closed   = 1 << 2;

chen::reactor::reactor(int count) : _count(count)
{
    // create udp to recv wake message
    this->_wake.reset(AF_INET, SOCK_DGRAM);

    if (this->_wake.bind(inet_address("127.0.0.1:0")))
        throw std::system_error(sys::error(), "poll: failed to bind on wake socket");

    if (this->_wake.nonblocking(true))
        throw std::system_error(sys::error(), "poll: failed to make nonblocking on wake socket");

    this->set(this->_wake.native(), nullptr, ModeRead, 0);
}

chen::reactor::~reactor()
{
}

// modify
void chen::reactor::set(handle_t fd, callback cb, int mode, int flag)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    // register event
    auto find = std::find_if(this->_cache.begin(), this->_cache.end(), [=] (::pollfd &item) {
        return item.fd == fd;
    });

    if (find == this->_cache.end())
        find = this->_cache.insert(this->_cache.end(), ::pollfd());

    find->fd = fd;
    find->events = 0;

    if (mode & ModeRead)
        find->events |= POLLIN;

    if (mode & ModeWrite)
        find->events |= POLLOUT;

    this->_flags[fd] = flag;

    // register callback
    this->_store[fd] = cb;
}

void chen::reactor::del(handle_t fd)
{
    std::lock_guard<std::mutex> lock(this->_mutex);
    
    // delete callback
    this->_store.erase(fd);

    // delete flags
    this->_flags.erase(fd);

    // delete event
    std::remove_if(this->_cache.begin(), this->_cache.end(), [=] (::pollfd &item) {
        return item.fd == fd;
    });
}

// run
void chen::reactor::run()
{
    for (std::error_code code; !code || (code == std::errc::interrupted); code = this->once())
        ;
}

std::error_code chen::reactor::once(double timeout)
{
    // poll events
    std::vector<::pollfd> cache;

    {
        std::lock_guard<std::mutex> lock(this->_mutex);
        cache = this->_cache;  // avoid race condition
    }

    int result = ::WSAPoll(cache.data(), cache.size(), timeout < 0 ? -1 : static_cast<int>(timeout * 1000));

    if (result <= 0)
    {
        if (!result)
            return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
        else
            throw std::system_error(sys::error(), "poll: failed to poll event");
    }

    // events on the same fd will be notified only once
    for (auto it = cache.begin(); it != cache.end(); ++it)
    {
        auto &item = *it;

        // continue if revents is 0
        if (!item.revents)
            continue;

        // user request to stop
        if (item.fd == this->_wake.native())
        {
            char dummy;
            basic_address addr;

            while (this->_wake.recvfrom(&dummy, 1, addr) >= 0)
                ;

            return std::make_error_code(std::errc::operation_canceled);
        }

        // invoke callback
        callback cb;

        {
            std::lock_guard<std::mutex> lock(this->_mutex);
            cb = this->_store[item.fd];
        }

        if (cb)
            cb(this->type(item.events));
    }

    return {};
}

void chen::reactor::stop()
{
    // notify wake message via socket
    basic_socket s(AF_INET, SOCK_DGRAM);
    
    // since it's a new socket, data should be written to buffer immediately
    if (s.nonblocking(true) || (s.sendto("\n", 1, this->_wake.sock()) != 1))
        throw std::system_error(sys::error(), "poll: failed to wake the poll");
}

// misc
chen::reactor::Type chen::reactor::type(int events)
{
    // check events, multiple events maybe occur
    if ((events & POLLERR) || (events & POLLHUP))
    {
        return Closed;
    }
    else
    {
        chen::reactor::Type ret = 0;

        if (events & POLLIN)
            ret |= Readable;

        if (events & POLLOUT)
            ret |= Writable;

        return ret;
    }
}

#endif