/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_socket.hpp>

namespace chen
{
    template <typename T>
    inline typename basic_socket<T>::address_type basic_socket<T>::peer() const noexcept
    {
        return this->_socket.peer();
    }

    template <typename T>
    inline typename basic_socket<T>::address_type basic_socket<T>::sock() const noexcept
    {
        return this->_socket.sock();
    }

    template <typename T>
    inline std::error_code basic_socket<T>::nonblocking(bool enable) noexcept
    {
        return this->_socket.nonblocking(enable);
    }

    template <typename T>
    inline typename basic_socket<T>::option_type basic_socket<T>::option() noexcept
    {
        return option_type(*this);
    }

    template <typename T>
    inline bool basic_socket<T>::valid() const noexcept
    {
        return this->_socket.valid();
    }

    template <typename T>
    inline basic_socket<T>::operator bool() const noexcept
    {
        return this->_socket;
    }

    template <typename T>
    inline socket_t basic_socket<T>::native() const noexcept
    {
        return this->_socket.native();
    }

    template <typename T>
    inline std::size_t basic_socket<T>::available() const noexcept
    {
        return this->_socket.available();
    }

    template <typename T>
    inline int basic_socket<T>::family() const noexcept
    {
        return this->_socket.family();
    }

    template <typename T>
    inline int basic_socket<T>::type() const noexcept
    {
        return this->_socket.type();
    }

    template <typename T>
    inline int basic_socket<T>::protocol() const noexcept
    {
        return this->_socket.protocol();
    }
}