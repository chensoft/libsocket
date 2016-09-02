/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/base_common.hpp>

// -----------------------------------------------------------------------------
// basic
chen::endpoint chen::common::local() const
{
    return this->_socket.local();
}

chen::endpoint chen::common::remote() const
{
    return this->_socket.remote();
}

bool chen::common::nonblocking() const
{
    return this->_socket.nonblocking();
}

bool chen::common::nonblocking(bool enable)
{
    return this->_socket.nonblocking(enable);
}

bool chen::common::empty() const
{
    return this->_socket.empty();
}

chen::common::operator bool() const
{
    return this->_socket;
}

chen::socket_t chen::common::native() const
{
    return this->_socket.native();
}