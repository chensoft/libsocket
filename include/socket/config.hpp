/**
 * Created by Jian Chen
 * @since  2016.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifndef _WIN32

// For Unix-like systems
#include <netinet/tcp.h>  // TCP macros
#include <netinet/in.h>   // IPv4 & IPv6
#include <sys/socket.h>   // socket
#include <sys/types.h>    // types
#include <sys/ioctl.h>    // ioctl
#include <unistd.h>       // close
#include <netdb.h>        // getaddrinfo
#include <fcntl.h>        // non-blocking

namespace chen
{
    typedef ::ssize_t  ssize_t;  // ssize_t type
    typedef int       socket_t;  // socket handle type
    typedef socklen_t option_t;  // socket option size

    constexpr int invalid_socket = -1;  // invalid file descriptor
}

#else

// For Windows
#include <winsock2.h>  // socket
#include <ws2tcpip.h>  // getaddrinfo
#include <windows.h>

namespace chen
{
    typedef SSIZE_T ssize_t;  // ssize_t type
    typedef SOCKET socket_t;  // socket handle type
    typedef int    option_t;  // socket option size

    constexpr SOCKET invalid_socket = INVALID_SOCKET;  // invalid socket value
}

#endif

// Global Initialization
#include <socket/sys/startup.hpp>