/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_server.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// server
chen::tcp::server::server(ip::address::Type family) : basic(family)
{
    this->nonblocking(true);
}

// control
void chen::tcp::server::start(const net::endpoint &ep)
{
    // todo how to handle socket being bind and listen before start
    // todo throw exception if already start
    if (this->bind(ep))
        throw std::system_error(sys::error(), "tcp: failed to bind address");

    if (this->listen())
        throw std::system_error(sys::error(), "tcp: failed to listen on address");

    this->_proactor.read(this, 0);
    this->_proactor.start();
}

void chen::tcp::server::stop()
{
    this->_proactor.stop();
}

// callback
void chen::tcp::server::attach(std::function<void (chen::tcp::server &s, std::shared_ptr<chen::tcp::conn> conn)> callback)
{
    this->_callback = callback;
}

void chen::tcp::server::detach()
{
    this->_callback = nullptr;
}

void chen::tcp::server::notify(std::shared_ptr<chen::tcp::conn> &&conn)
{
    this->_connections.insert(conn);

    if (this->_callback)
        this->_callback(*this, std::move(conn));
}

// event
void chen::tcp::server::onRead(std::vector<std::uint8_t> data, net::endpoint ep, std::error_code error)
{
    if (error)
        return this->stop();

    socket_t fd;

    if (this->_handle.accept(fd))
        return this->stop();

    // todo use edge trigger not re-register events every time
    this->_proactor.read(this, 0);

    this->notify(std::make_shared<tcp::conn>(fd, this));
}

void chen::tcp::server::onWrite(std::size_t size, net::endpoint ep, std::error_code error)
{
    // unused
}

void chen::tcp::server::onEnd()
{
    // stop if error occur
    this->stop();
}

// bind
std::error_code chen::tcp::server::bind(const net::endpoint &ep)
{
    return this->_handle.bind(ep);
}

std::error_code chen::tcp::server::listen()
{
    return this->_handle.listen();
}

std::error_code chen::tcp::server::listen(int backlog)
{
    return this->_handle.listen(backlog);
}