/**
 * Created by Jian Chen
 * @since  2016.11.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/core/poller.hpp>
#include <chen/sys/sys.hpp>
#include <algorithm>

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
	item.first.events = 0;

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
	if (!count || this->_fds.empty() || ((this->_fds.size() == 1) && this->_up))  // ignore if it's empty or just a wakeup socket in it
		return 0;

	// create socket to receive close event
	if (!this->_up)
	{
		this->_up.reset(AF_INET, SOCK_DGRAM);
		this->set(this->_up.native(), OpcodeRead);
	}

	// temporary use only
	std::vector<::pollfd> scan(this->_fds.size());

	auto data = this->_fds;
	auto copy = 0;

	for (auto &pair : data)
		scan[copy++] = pair.second.first;

	// wait for events
	this->_wk = true;

	auto result = ::WSAPoll(scan.data(), static_cast<ULONG>(data.size()), timeout < 0 ? -1 : static_cast<int>(timeout * 1000));

	this->_wk = false;

	// check error status
	if (result <= 0)
	{
		// zero means timeout
		if (!result)
			return 0;
		else
			throw std::system_error(sys::error(), "poller: failed to poll event");
	}

	// user request to stop
	if (!this->_up)
		return 0;

	// check return data
	auto length = cache.size();

	for (std::size_t i = 0, l = data.size(), c = count; (i < l) && c; ++i)
	{
		auto &event = scan[i];
		if (event.revents == 0)
			continue;

		// check events, multiple events maybe occur
		auto insert = [&](Event code) {
			// remove fd if Ended event occurs or flag is once
			if ((code == Event::Ended) || (data[event.fd].second & FlagOnce))
				this->del(event.fd);

			if (i < length)
				cache[i] = Data(event.fd, code);
			else
				cache.emplace_back(Data(event.fd, code));

			--c;
		};

		if ((event.revents & POLLERR) || (event.revents & POLLHUP))
		{
			insert(Event::Ended);
		}
		else
		{
			if (event.revents & POLLIN)
				insert(Event::Readable);

			if ((event.revents & POLLOUT) && c)
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

	// notify wake message via socket's close event
	this->_up.close();
}

#endif