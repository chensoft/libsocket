/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_conn.hpp>

// -----------------------------------------------------------------------------
// conn
chen::tcp::conn::conn(socket &&sock) : basic(std::move(sock))
{
}

ssize_t chen::tcp::conn::send(const std::vector<std::uint8_t> &data, int flags)
{
    return this->send(data.data(), data.size(), flags);
}

std::vector<std::uint8_t> chen::tcp::conn::recv(std::size_t size, int flags)
{
    std::vector<std::uint8_t> ret(size);

    auto count = this->recv(ret, size, flags);
    ret.resize(static_cast<std::size_t>(count >= 0 ? count : 0));

    return ret;
}

void chen::tcp::conn::disconnect()
{
    this->close();
}