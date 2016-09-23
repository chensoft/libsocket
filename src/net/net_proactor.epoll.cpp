/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/net/net_proactor.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// helper
namespace
{
//    void register_write(int k, int fd, void *data)
//    {
//    }
//
//    void register_read(int k, int fd, void *data)
//    {
//    }
}


// -----------------------------------------------------------------------------
// proactor
chen::net::proactor::proactor()
{
    // todo
}

chen::net::proactor::~proactor()
{
}

void chen::net::proactor::send(net::socket *ptr, std::vector<std::uint8_t> &&data)
{
}

void chen::net::proactor::recv(net::socket *ptr, std::size_t size)
{
}

void chen::net::proactor::remove(net::socket *ptr)
{
}

void chen::net::proactor::start()
{
}

void chen::net::proactor::stop()
{
}

#endif