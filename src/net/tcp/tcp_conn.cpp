/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/tcp/tcp_handler.hpp>
#include <cstring>

// -----------------------------------------------------------------------------
// conn
chen::tcp::conn::conn(basic_socket &&s, std::unique_ptr<handler> &&h) : _handler(std::move(h))
{
    this->reset(std::move(s));
    this->nonblocking(true);
}

chen::tcp::conn::~conn()
{
    if (this->_socket)
        this->_socket.close();
}

void chen::tcp::conn::disconnect()
{
    // we shutdown the socket here, server will
    // receive the ended event and destroy this
    if (this->_socket)
        this->_socket.shutdown();

    this->_connected = false;
}

// read
void chen::tcp::conn::readSome()
{
    this->readSome(4096);  // 4096 is just a magic number
}

void chen::tcp::conn::readSome(std::size_t size)
{
    this->_policy.reset(new read_some_policy(size));
    this->receive();
}

void chen::tcp::conn::readAll()
{
    this->_policy.reset(new read_all_policy);
    this->receive();
}

void chen::tcp::conn::readLine()
{
    this->_policy.reset(new read_line_policy);
    this->receive();
}

void chen::tcp::conn::readExact(std::size_t size)
{
    this->_policy.reset(new read_exact_policy(size));
    this->receive();
}

void chen::tcp::conn::readUntil(const std::string &text)
{
    this->_policy.reset(new read_until_policy(text));
    this->receive();
}

// write
void chen::tcp::conn::write(const char *text)
{
    this->write(text, ::strlen(text));
}

void chen::tcp::conn::write(const std::string &text)
{
    this->write(text.data(), text.size());
}

void chen::tcp::conn::write(const void *data, std::size_t size)
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
        this->_handler->onWrite(*this, len);
    }
    else
    {
        // store remaining data into buf
        this->_buf_write.insert(this->_buf_write.end(), ptr + len, ptr + size);

        // partial data has been sent
        if (len)
            this->_handler->onWrite(*this, len);
    }
}

// property
bool chen::tcp::conn::isConnected() const
{
    return this->_connected;
}

bool chen::tcp::conn::isDisconnect() const
{
    return !this->_connected;
}

chen::socket_t chen::tcp::conn::native() const
{
    return this->_socket.native();
}

// receive
void chen::tcp::conn::receive()
{
    if (!this->_policy)
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

void chen::tcp::conn::receive(read_some_policy &policy)
{
    if (this->_buf_read.size() <= policy.size)
    {
        // for performance, directly move buf if size <= policy.size
        this->_handler->onRead(*this, std::move(this->_buf_read));
    }
    else
    {
        // copy and move buffer data
        std::vector<std::uint8_t> data(this->_buf_read.begin(), this->_buf_read.begin() + policy.size);
        this->_buf_read.erase(this->_buf_read.begin(), this->_buf_read.begin() + policy.size);

        this->_handler->onRead(*this, std::move(data));
    }
}

void chen::tcp::conn::receive(read_all_policy &policy)
{
    this->_handler->onRead(*this, std::move(this->_buf_read));
}

void chen::tcp::conn::receive(read_line_policy &policy)
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

            return this->_handler->onRead(*this, std::move(data));
        }
    }

    // pass rest of the data if eof
    if (this->isDisconnect())
        return this->_handler->onRead(*this, std::move(this->_buf_read));

    // caching last search position
    policy.pos = len;
}

void chen::tcp::conn::receive(read_exact_policy &policy)
{
    auto size = this->_buf_read.size();

    if (size == policy.size)
    {
        // for performance, directly move buf if size == policy.size
        this->_handler->onRead(*this, std::move(this->_buf_read));
    }
    else if (size > policy.size)
    {
        // copy and move buffer data
        std::vector<std::uint8_t> data(this->_buf_read.begin(), this->_buf_read.begin() + policy.size);
        this->_buf_read.erase(this->_buf_read.begin(), this->_buf_read.begin() + policy.size);

        this->_handler->onRead(*this, std::move(data));
    }
}

void chen::tcp::conn::receive(read_until_policy &policy)
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

            return this->_handler->onRead(*this, std::move(data));
        }
    }

    policy.pos = i;
}

// event
void chen::tcp::conn::onAccepted()
{
    this->_handler->onAccepted(*this);
}

void chen::tcp::conn::onReadable()
{
    this->receive();
}

void chen::tcp::conn::onWritable()
{
    // try to send all data
    if (this->_buf_write.empty())
        return;

    auto ret = this->_socket.send(this->_buf_write.data(), this->_buf_write.size());
    if (ret <= 0)
        return;

    // notify write callback
    this->_buf_write.erase(this->_buf_write.begin(), this->_buf_write.begin() + ret);
    this->_handler->onWrite(*this, static_cast<std::size_t>(ret));
}

void chen::tcp::conn::onEnded()
{
    // read rest of the data
    this->receive();

    // connection broken
    this->_connected = false;
    this->_handler->onDisconnect(*this);
}