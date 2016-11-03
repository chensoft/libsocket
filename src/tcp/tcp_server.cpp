/**
 * Created by Jian Chen
 * @since  2016.11.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_resolver.hpp>
#include <socket/tcp/tcp_server.hpp>
#include <chen/base/str.hpp>

// -----------------------------------------------------------------------------
// server
chen::tcp::server::server(net::runloop &runloop, std::uint16_t port, ip::address::Type type) : _runloop(runloop)
{
    this->_local.port(port);
    this->_local.addr(ip::address(type));

    this->reset(this->_local.addr().type());
}

chen::tcp::server::server(net::runloop &runloop, const char *mixed) : server(runloop, mixed, ip::address::Type::None)
{
}

chen::tcp::server::server(net::runloop &runloop, const std::string &mixed, ip::address::Type type) : _runloop(runloop)
{
    auto ret = net::resolver::resolve(mixed, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: server resolve address '%s' fail", mixed.c_str()));

    this->_local = ret.front();

    this->reset(this->_local.addr().type());
}

chen::tcp::server::server(net::runloop &runloop, const std::string &host, std::uint16_t port, ip::address::Type type) : _runloop(runloop)
{
    auto ret = net::resolver::resolve(host, port, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: server resolve address '%s' fail", host.c_str()));

    this->_local = ret.front();

    this->reset(this->_local.addr().type());
}

chen::tcp::server::server(net::runloop &runloop, const std::string &host, const std::string &service, ip::address::Type type) : _runloop(runloop)
{
    auto ret = net::resolver::resolve(host, service, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: server resolve address '%s' or service '%s' fail", host.c_str(), service.c_str()));

    this->_local = ret.front();

    this->reset(this->_local.addr().type());
}

chen::tcp::server::server(net::runloop &runloop, const net::endpoint &ep) : _local(ep), _runloop(runloop)
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
    if (this->_handle)
        this->_runloop.del(this->_handle.native());

    this->_handle.shutdown();
    this->_handle.close();

    this->_running = false;
}

void chen::tcp::server::pause()
{
    // todo
}

void chen::tcp::server::resume()
{
    // todo
}

// property
bool chen::tcp::server::isRunning()
{
    return this->_running;
}

chen::net::endpoint chen::tcp::server::local() const
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
    if (!this->_handle)
        this->reset(this->_local.addr().type());

    this->nonblocking(true);
    this->option().reuseaddr(true);

    // bind and listen
    auto err = this->_handle.bind(this->_local);
    if (err)
        throw std::system_error(err, "tcp: server bind error");

    err = this->_handle.listen(backlog);
    if (err)
        throw std::system_error(err, "tcp: server listen error");

    // listen events using runloop
    this->_runloop.set(this->_handle.native(),
                       net::runloop::OpcodeRead | net::runloop::OpcodeWrite,
                       net::runloop::FlagEdge,
                       std::bind(&server::onEvent, this, std::placeholders::_1));

    this->_running = true;
}

// server
void chen::tcp::server::onReadable()
{
    bsd::socket s = this->_handle.accept();
    if (!s)
        return;

    this->_store.emplace_back(std::unique_ptr<conn>(new conn(this->_runloop, std::move(s), this->_factory())));
}

void chen::tcp::server::onWritable()
{
}

void chen::tcp::server::onEnded()
{
}

void chen::tcp::server::onEvent(net::runloop::Event type)
{
    switch (type)
    {
        case net::runloop::Event::Read:
            return this->onReadable();

        case net::runloop::Event::Write:
            return this->onWritable();

        case net::runloop::Event::End:
            return this->onEnded();
    }
}