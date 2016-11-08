/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
//#include <socket/bsd/dgram_handler.hpp>
//#include <cstring>
//
//// -----------------------------------------------------------------------------
//// dgram_client
//chen::dgram_client::dgram_client(runloop &loop, int family, int protocol) : _runloop(loop)
//{
//    this->_socket.reset(family, SOCK_DGRAM, protocol);
//    this->_socket.nonblocking(true);
//
//    this->_runloop.set(this->_socket.native(), runloop::OpcodeRW, runloop::FlagEdge,
//                       std::bind(&dgram_client::onEvent, this, std::placeholders::_1));
//}
//
//chen::dgram_client::~dgram_client()
//{
//    if (this->_socket)
//    {
//        this->_runloop.del(this->_socket.native());
//
//        this->_socket.shutdown();
//        this->_socket.close();
//    }
//}
//
//// handler
//void chen::dgram_client::attach(dgram_client_handler *handler)
//{
//    this->_handler = handler;
//}
//
//void chen::dgram_client::detach()
//{
//    this->_handler = nullptr;
//}
//
//void chen::dgram_client::notify(std::vector<std::uint8_t> &&data, const chen::basic_address &addr)
//{
//    if (this->_handler)
//        this->_handler->onRead(*this, std::move(data), std::move(addr));
//}
//
//void chen::dgram_client::notify(std::size_t size, const chen::basic_address &addr)
//{
//    if (this->_handler)
//        this->_handler->onWrite(*this, size, addr);
//}
//
//// event
//void chen::dgram_client::onReadable()
//{
//}
//
//void chen::dgram_client::onWritable()
//{
//}
//
//void chen::dgram_client::onEnded()
//{
//}
//
//void chen::dgram_client::onEvent(runloop::Event type)
//{
//    switch (type)
//    {
//        case runloop::Event::Readable:
//            return this->onReadable();
//
//        case runloop::Event::Writable:
//            return this->onWritable();
//
//        case runloop::Event::Ended:
//            return this->onEnded();
//    }
//}