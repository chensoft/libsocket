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
#include <sys/socket.h>   // socket defines
#include <sys/types.h>    // types
#include <sys/ioctl.h>    // ioctl
#include <unistd.h>       // socket close()
#include <netdb.h>        // getaddrinfo
#include <fcntl.h>        // non-blocking

namespace chen
{
    typedef int socket_t;        // socket handle type
    typedef socklen_t option_t;  // socket option size

    static constexpr int invalid_handle = -1;  // invalid file descriptor
}

#else

// For Windows
#include <winsock2.h>  // socket defines
#include <ws2tcpip.h>  // getaddrinfo
#include <windows.h>

namespace chen
{
    typedef SOCKET socket_t;  // socket handle type
    typedef int option_t;     // socket option size

    static constexpr int invalid_handle = INVALID_HANDLE_VALUE;  // invalid handle value
}

#endif