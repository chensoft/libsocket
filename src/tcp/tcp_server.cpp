/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_server.hpp>

// -----------------------------------------------------------------------------
// server
//std::error_code chen::tcp::server::start(const endpoint &ep)
//{
//    this->_socket.reset(ep.addr(), SOCK_STREAM);
//
//    if (!this->_socket.bind(ep))
//    {
//        this->notify(this, sys::error());
//        return false;
//    }
//
//    if (!this->_socket.listen())
//    {
//        this->notify(this, sys::error());
//        return false;
//    }
//
//    this->_exit = false;
//
//    while (true)
//    {
//        // accept new request
//        auto sock = this->_socket.accept();
//
//        // todo
//        if (sock)
//            this->notify(std::move(sock));
//        else
//            this->notify(this, sys::error());
//
//        // check exit flag
//        // todo use poll instead of flag
//        if (this->_exit)
//            break;
//    }
//
//    return {};
//}
//
//std::error_code chen::tcp::server::start(const ip::address &addr, std::uint16_t port)
//{
//    return this->start(endpoint(addr, port));
//}
//
//std::error_code chen::tcp::server::stop()
//{
////    // todo
////    this->_exit = true;
////    this->_socket.close();
//    return {};
//}