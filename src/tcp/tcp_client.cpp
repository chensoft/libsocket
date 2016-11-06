/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_resolver.hpp>
#include <socket/tcp/tcp_client.hpp>
#include <chen/base/str.hpp>
#include <cstring>

// -----------------------------------------------------------------------------
// client
chen::tcp::client::client(runloop &runloop) : _runloop(runloop)
{
}

chen::tcp::client::~client()
{
    this->disconnect();
}

// connection
void chen::tcp::client::connect(const char *mixed)
{
    this->connect(mixed, ip_address::Type::None);
}

void chen::tcp::client::connect(const std::string &mixed, ip_address::Type type)
{
    auto ret = inet_resolver::resolve(mixed, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: client resolve address '%s' fail", mixed.c_str()));

    this->connect(ret.front());
}

void chen::tcp::client::connect(const std::string &host, std::uint16_t port, ip_address::Type type)
{
    auto ret = inet_resolver::resolve(host, port, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: client resolve address '%s' fail", host.c_str()));

    this->connect(ret.front());
}

void chen::tcp::client::connect(const std::string &host, const std::string &service, ip_address::Type type)
{
    auto ret = inet_resolver::resolve(host, service, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: client resolve address '%s' or service '%s' fail", host.c_str(), service.c_str()));

    this->connect(ret.front());
}

void chen::tcp::client::connect(const inet_address &addr)
{
    // disconnect is if connected
    if (!this->isDisconnect())
        this->disconnect();

    // recreate socket and use nb mode
    this->reset(addr.addr().type());
    this->nonblocking(true);

    // listen events using runloop
    this->_runloop.set(this->_socket.native(), runloop::OpcodeRW, runloop::FlagEdge,
                       std::bind(&client::onEvent, this, std::placeholders::_1));

    // connect to remote host
    this->_remote = addr;
    this->_state  = State::Connecting;

    auto error = this->_socket.connect(addr);

    if (error != std::errc::operation_in_progress)
    {
        this->disconnect();
        this->notify(connected_event(this->_remote, error));  // notify connected error
    }
}

void chen::tcp::client::reconnect()
{
    this->connect(this->_remote);
}

void chen::tcp::client::disconnect()
{
    if (this->_socket)
    {
        this->_runloop.del(this->_socket.native());

        this->_socket.shutdown();
        this->_socket.close();
    }

    this->_state = State::Disconnect;
    this->_policy.reset();

    this->_buf_read.clear();
    this->_buf_write.clear();
}

// read
void chen::tcp::client::readSome()
{
    this->readSome(4096);  // 4096 is just a magic number
}

void chen::tcp::client::readSome(std::size_t size)
{
    this->_policy.reset(new read_some_policy(size));
    this->receive();
}

void chen::tcp::client::readAll()
{
    this->_policy.reset(new read_all_policy);
    this->receive();
}

void chen::tcp::client::readLine()
{
    this->_policy.reset(new read_line_policy);
    this->receive();
}

void chen::tcp::client::readExact(std::size_t size)
{
    this->_policy.reset(new read_exact_policy(size));
    this->receive();
}

void chen::tcp::client::readUntil(const std::string &text)
{
    this->_policy.reset(new read_until_policy(text));
    this->receive();
}

// write
void chen::tcp::client::write(const char *text)
{
    this->write(text, ::strlen(text));
}

void chen::tcp::client::write(const std::string &text)
{
    this->write(text.data(), text.size());
}

void chen::tcp::client::write(const void *data, std::size_t size)
{
    auto ptr = static_cast<const std::uint8_t*>(data);

    // if pending data exist, it means we are waiting for writable event
    // if socket is not established, we wait for the connection to be established
    if (!this->_buf_write.empty() || !this->isConnected())
    {
        this->_buf_write.insert(this->_buf_write.end(), ptr, ptr + size);
        return;
    }

    // try to use send directly first
    auto ret = this->_socket.send(data, size);
    auto len = static_cast<std::size_t>(ret >= 0 ? ret : 0);

    if (len == size)
    {
        // all data has been sent out
        this->notify(write_event(len));
    }
    else
    {
        // store remaining data into buf
        this->_buf_write.insert(this->_buf_write.end(), ptr + len, ptr + size);

        // partial data has been sent
        if (len)
            this->notify(write_event(len));
    }
}

// property
bool chen::tcp::client::isConnecting() const
{
    return this->_state == State::Connecting;
}

bool chen::tcp::client::isConnected() const
{
    return this->_state == State::Connected;
}

bool chen::tcp::client::isDisconnect() const
{
    return this->_state == State::Disconnect;
}

chen::inet_address chen::tcp::client::remote() const
{
    return this->_remote;
}

// event
void chen::tcp::client::attach(std::function<void (connected_event event)> callback)
{
    this->_cb_connected = callback;
}

void chen::tcp::client::attach(std::function<void (disconnect_event event)> callback)
{
    this->_cb_disconnect = callback;
}

void chen::tcp::client::attach(std::function<void (read_event event)> callback)
{
    this->_cb_read = callback;
}

void chen::tcp::client::attach(std::function<void (write_event event)> callback)
{
    this->_cb_write = callback;
}

void chen::tcp::client::detach(Event type)
{
    switch (type)
    {
        case Event::Connected:
            this->_cb_connected = nullptr;
            break;

        case Event::Disconnect:
            this->_cb_disconnect = nullptr;
            break;

        case Event::Read:
            this->_cb_read = nullptr;
            break;

        case Event::Write:
            this->_cb_write = nullptr;
            break;
    }
}

// notify
void chen::tcp::client::notify(tcp::connected_event &&event)
{
    this->_state = !event.err ? State::Connected : State::Disconnect;

    if (this->_cb_connected)
        this->_cb_connected(std::move(event));
}

void chen::tcp::client::notify(tcp::disconnect_event &&event)
{
    this->_state = State::Disconnect;

    if (this->_cb_disconnect)
        this->_cb_disconnect(std::move(event));
}

void chen::tcp::client::notify(tcp::read_event &&event)
{
    if (this->_policy->once)
        this->_policy.reset();  // remove it since it's a one time policy

    if (this->_cb_read)
        this->_cb_read(std::move(event));
}

void chen::tcp::client::notify(tcp::write_event &&event)
{
    if (this->_cb_write)
        this->_cb_write(std::move(event));
}

// receive
void chen::tcp::client::receive()
{
    if (!this->_policy || this->isConnecting())
        return;

    // read all available data to buf
    std::uint8_t buf[4096];  // 4096 is just a magic number, no specific meaning

    while (true)
    {
        auto len = this->_socket.recv(buf, 4096);
        if (len <= 0)
            break;

        this->_buf_read.insert(this->_buf_read.end(), buf, buf + len);
    }

    if (this->_buf_read.empty())
        return;

    // check and dispatch read event
    auto ptr = this->_policy.get();

    switch (ptr->type)
    {
        case policy::Type::Some:
            this->receive(*static_cast<read_some_policy*>(ptr));
            break;

        case policy::Type::All:
            this->receive(*static_cast<read_all_policy*>(ptr));
            break;

        case policy::Type::Line:
            this->receive(*static_cast<read_line_policy*>(ptr));
            break;

        case policy::Type::Exact:
            this->receive(*static_cast<read_exact_policy*>(ptr));
            break;

        case policy::Type::Until:
            this->receive(*static_cast<read_until_policy*>(ptr));
            break;
    }
}

void chen::tcp::client::receive(read_some_policy &policy)
{
    if (this->_buf_read.size() <= policy.size)
    {
        // for performance, directly move buf if size <= policy.size
        this->notify(read_event(std::move(this->_buf_read)));
    }
    else
    {
        // copy and move buffer data
        std::vector<std::uint8_t> data(this->_buf_read.begin(), this->_buf_read.begin() + policy.size);
        this->_buf_read.erase(this->_buf_read.begin(), this->_buf_read.begin() + policy.size);

        this->notify(read_event(std::move(data)));
    }
}

void chen::tcp::client::receive(read_all_policy &policy)
{
    this->notify(read_event(std::move(this->_buf_read)));
}

void chen::tcp::client::receive(read_line_policy &policy)
{
    auto ptr = (const char*)this->_buf_read.data();
    auto len = this->_buf_read.size();
    auto num = 0;

    for (std::size_t i = policy.pos; i < len; ++i)
    {
        auto ch = ptr[i];

        if (ch == '\n')
        {
            num = 1;

            // further checking the "\r\n" line delimiter
            if ((i > policy.pos) && (ptr[i - 1] == '\r'))
                num = 2;
        }

        if (num)
        {
            // notice, don't include the delimiter in returned data
            std::vector<std::uint8_t> data(ptr, ptr + i + 1 - num);
            this->_buf_read.erase(this->_buf_read.begin(), this->_buf_read.begin() + i + 1);

            return this->notify(read_event(std::move(data)));
        }
    }

    // pass rest of the data if eof
    if (this->isDisconnect())
        return this->notify(read_event(std::move(this->_buf_read)));

    // caching last search position
    policy.pos = len;
}

void chen::tcp::client::receive(read_exact_policy &policy)
{
    auto size = this->_buf_read.size();

    if (size == policy.size)
    {
        // for performance, directly move buf if size == policy.size
        this->notify(read_event(std::move(this->_buf_read)));
    }
    else if (size > policy.size)
    {
        // copy and move buffer data
        std::vector<std::uint8_t> data(this->_buf_read.begin(), this->_buf_read.begin() + policy.size);
        this->_buf_read.erase(this->_buf_read.begin(), this->_buf_read.begin() + policy.size);

        this->notify(read_event(std::move(data)));
    }
}

void chen::tcp::client::receive(read_until_policy &policy)
{
    auto len_buffer = this->_buf_read.size();
    auto len_policy = policy.text.size();

    if (len_buffer < len_policy)
        return;

    auto ptr_buffer = (const char*)this->_buf_read.data();
    auto ptr_policy = policy.text.data();

    std::size_t i = policy.pos;

    while (i < len_buffer)
    {
        std::size_t j = 0;

        for (; j < len_policy; ++j)
        {
            if (ptr_policy[j] != ptr_buffer[i])
            {
                i += j + 1;
                break;
            }
            else
            {
                ++i;
            }
        }

        if (j == len_policy)
        {
            // notice, don't include the delimiter in returned data
            std::vector<std::uint8_t> data(ptr_buffer, ptr_buffer + i - len_policy);
            this->_buf_read.erase(this->_buf_read.begin(), this->_buf_read.begin() + i);

            return this->notify(read_event(std::move(data)));
        }
    }

    policy.pos = i;
}

// event
void chen::tcp::client::onReadable()
{
    // todo test not received connected, but remote send a data to me
    if (this->isConnected())
        this->receive();
    else
        throw std::runtime_error("tcp: client not connected but received read event");
}

void chen::tcp::client::onWritable()
{
    // notify connected event
    if (this->isConnecting())
        this->notify(connected_event(this->_remote, {}));

    // try to send all data
    if (this->_buf_write.empty())
        return;

    auto ret = this->_socket.send(this->_buf_write.data(), this->_buf_write.size());
    if (ret <= 0)
        return;

    // notify write callback
    this->_buf_write.erase(this->_buf_write.begin(), this->_buf_write.begin() + ret);
    this->notify(write_event(static_cast<std::size_t>(ret)));
}

void chen::tcp::client::onEnded()
{
    auto error = this->option().error();

    if (this->isConnecting())
    {
        // connection refused or reset by peer
        // @see http://man7.org/linux/man-pages/man2/connect.2.html about EINPROGRESS
        this->disconnect();
        this->notify(connected_event(this->_remote, error));
    }
    else if (this->isConnected())
    {
        // read rest of the data
        this->_state = State::Disconnect;  // some receive methods need to know if eof occurs
        this->receive();

        // connection broken
        this->disconnect();
        this->notify(disconnect_event(error));
    }
    else
    {
        throw std::runtime_error("tcp: client in disconnect state but received end event");
    }
}

void chen::tcp::client::onEvent(runloop::Event type)
{
    switch (type)
    {
        case runloop::Event::Readable:
            return this->onReadable();

        case runloop::Event::Writable:
            return this->onWritable();

        case runloop::Event::Ended:
            return this->onEnded();
    }
}