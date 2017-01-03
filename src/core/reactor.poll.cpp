/**
 * Created by Jian Chen
 * @since  2016.12.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
//#ifdef _WIN32

#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>

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
}

chen::reactor::~reactor()
{
}

//// modify
//void chen::service_poller::set(handle_t fd, void *data, int opcode, int flag)
//{
//    auto find = std::find_if(this->_fds.begin(), this->_fds.end(), [&] (::pollfd &item) {
//        return item.fd == fd;
//    });
//
//    if (find == this->_fds.end())
//        find = this->_fds.insert(this->_fds.end(), ::pollfd());
//
//    find->fd = fd;
//    find->events = POLLRDHUP;
//
//    if (opcode & OpcodeRead)
//        find->events |= POLLIN;
//
//    if (opcode & OpcodeWrite)
//        find->events |= POLLOUT;
//
//    this->_map[fd] = Detail(flag, data);
//}
//
//void chen::service_poller::del(handle_t fd)
//{
//    (void)std::remove_if(this->_fds.begin(), this->_fds.end(), [&] (::pollfd &item) {
//        return item.fd == fd;
//    });
//
//    this->_map.erase(fd);
//}
//
//std::size_t chen::service_poller::poll(std::vector<Data> &cache, std::size_t count, double timeout)
//{
//    // ignore if it's empty or just a wakeup socket in it
//    if (!count || this->_fds.empty() || (this->_fds.front().fd == this->_wake.native()))
//        return 0;
//
//    // reset wake event
//    if (!this->_wake)
//    {
//        this->_wake.reset(AF_INET, SOCK_DGRAM);
//        this->set(this->_wake.native(), nullptr, OpcodeRead, 0);
//    }
//
//    // temporary use only
//    auto fds = this->_fds;
//    auto map = this->_map;
//
//    // poll next events
//    auto result = ::poll(fds.data(), static_cast<unsigned>(fds.size()), timeout < 0 ? -1 : static_cast<int>(timeout * 1000));
//
//    if (result <= 0)
//    {
//        // EINTR maybe triggered by debugger, treat it as user request to stop
//        if ((errno == EINTR) || !result)
//            return 0;
//        else
//            throw std::system_error(sys::error(), "poller: failed to poll event");
//    }
//
//    // user request to stop
//    if (!this->_wake)
//        return 0;
//
//    // collect poll data
//    std::size_t origin = cache.size();
//    std::size_t number = 0;
//
//    for (std::size_t i = 0, l = fds.size(); i < l; ++i)
//    {
//        auto &event = fds[i];
//        if (event.revents == 0)
//            continue;
//
//        // check events, multiple events maybe occur
//        auto insert = [&] (Event code) {
//            // remove fd if flag is once
//            if (map[event.fd].flag & FlagOnce)
//                this->del(event.fd);
//
//            ++number;
//
//            if (i < origin)
//                cache[i] = Data(map[event.fd].data, code);
//            else
//                cache.emplace_back(Data(map[event.fd].data, code));
//        };
//
//        if ((event.revents & POLLRDHUP) || (event.revents & POLLERR) || (event.revents & POLLHUP))
//        {
//            insert(Type::Closed);
//        }
//        else
//        {
//            if (event.revents & POLLIN)
//                insert(Type::Readable);
//
//            if (event.revents & POLLOUT)
//                insert(Type::Writable);
//        }
//    }
//
//    return number;
//}
//
//std::vector<chen::service_poller::Data> chen::service_poller::poll(std::size_t count, double timeout)
//{
//    std::vector<chen::service_poller::Data> ret;
//    this->poll(ret, count, timeout);
//    return ret;
//}
//
//void chen::service_poller::stop()
//{
//    // notify wake message via socket's close event
//    this->_wake.close();
//}

//#endif