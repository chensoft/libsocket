/**
 * Created by Jian Chen
 * @since  2016.11.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_resolver.hpp>
#include <socket/tcp/tcp_server.hpp>
#include <socket/sys/runloop.hpp>
#include <chen/base/str.hpp>
#include <algorithm>

// -----------------------------------------------------------------------------
// server
chen::tcp::server::server(runloop &runloop, std::uint16_t port, ip_address::Type type) : _runloop(runloop)
{
    this->_local.port(port);
    this->_local.addr(ip_address(type));

    this->reset(this->_local.addr().type());
}

chen::tcp::server::server(runloop &runloop, const char *mixed) : server(runloop, mixed, ip_address::Type::None)
{
}

chen::tcp::server::server(runloop &runloop, const std::string &mixed, ip_address::Type type) : _runloop(runloop)
{
    auto ret = inet_resolver::resolve(mixed, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: server resolve address '%s' fail", mixed.c_str()));

    this->_local = ret.front();

    this->reset(this->_local.addr().type());
}

chen::tcp::server::server(runloop &runloop, const std::string &host, std::uint16_t port, ip_address::Type type) : _runloop(runloop)
{
    auto ret = inet_resolver::resolve(host, port, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: server resolve address '%s' fail", host.c_str()));

    this->_local = ret.front();

    this->reset(this->_local.addr().type());
}

chen::tcp::server::server(runloop &runloop, const std::string &host, const std::string &service, ip_address::Type type) : _runloop(runloop)
{
    auto ret = inet_resolver::resolve(host, service, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: server resolve address '%s' or service '%s' fail", host.c_str(), service.c_str()));

    this->_local = ret.front();

    this->reset(this->_local.addr().type());
}

chen::tcp::server::server(runloop &runloop, const inet_address &addr) : _local(addr), _runloop(runloop)
{
    this->reset(this->_local.addr().type());
}

chen::tcp::server::~server()
{
    this->stop();
}

// control
void chen::tcp::server::stop()
{
    if (this->_socket)
    {
        this->_runloop.del(this->_socket.native());

        this->_socket.shutdown();
        this->_socket.close();
    }

    this->_running = false;
}

// property
bool chen::tcp::server::isRunning()
{
    return this->_running;
}

chen::inet_address chen::tcp::server::local() const
{
    return this->_local;
}

// serve
void chen::tcp::server::listen(int backlog)
{
    // stop if running
    if (this->isRunning())
        this->stop();

    // reset socket
    if (!this->_socket)
        this->reset(this->_local.addr().type());

    this->nonblocking(true);
    this->option().reuseaddr(true);

    // bind and listen
    auto err = this->_socket.bind(this->_local);
    if (err)
        throw std::system_error(err, "tcp: server bind error");

    err = this->_socket.listen(backlog);
    if (err)
        throw std::system_error(err, "tcp: server listen error");

    // listen events using runloop
    this->_runloop.set(this->_socket.native(), runloop::OpcodeRW, runloop::FlagEdge,
                       std::bind(&server::onServerEvent, this, std::placeholders::_1));

    this->_running = true;
}

// server
void chen::tcp::server::onServerReadable()
{
    basic_socket s = this->_socket.accept();
    if (!s)
        return;

    auto func = this->_factory();
    auto &ptr = *this->_store.insert(this->_store.end(), std::unique_ptr<conn>(new conn(std::move(s), std::move(func))));
    this->_runloop.set(ptr->native(), runloop::OpcodeRW, runloop::FlagEdge,
                       std::bind(&server::onConnEvent, this, std::ref(ptr), std::placeholders::_1));

    ptr->onAccepted();
}

void chen::tcp::server::onServerWritable()
{
}

void chen::tcp::server::onServerEnded()
{
}

void chen::tcp::server::onServerEvent(runloop::Event type)
{
    switch (type)
    {
        case runloop::Event::Readable:
            return this->onServerReadable();

        case runloop::Event::Writable:
            return this->onServerWritable();

        case runloop::Event::Ended:
            return this->onServerEnded();
    }
}

// connection
void chen::tcp::server::onConnReadable(std::unique_ptr<conn> &c)
{
    c->onReadable();
}

void chen::tcp::server::onConnWritable(std::unique_ptr<conn> &c)
{
    c->onWritable();
}

void chen::tcp::server::onConnEnded(std::unique_ptr<conn> &c)
{
    // check if the connection is disconnected passively
    if (!c->isDisconnect())
        c->onEnded();

    // delete connection from runloop
    this->_runloop.del(c->native());

    // delete connection from connection pool
    auto it = std::find(this->_store.begin(), this->_store.end(), c);
    this->_store.erase(it);
}

void chen::tcp::server::onConnEvent(std::unique_ptr<conn> &c, runloop::Event type)
{
    switch (type)
    {
        case runloop::Event::Readable:
            return this->onConnReadable(c);

        case runloop::Event::Writable:
            return this->onConnWritable(c);

        case runloop::Event::Ended:
            return this->onConnEnded(c);
    }
}