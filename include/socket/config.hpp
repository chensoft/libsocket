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
#include <sys/socket.h>   // socket defines
#include <sys/ioctl.h>    // ioctl
#include <arpa/inet.h>    // IPv4 and IPv6
#include <unistd.h>       // socket close()
#include <netdb.h>        // getaddrinfo
#include <fcntl.h>        // non-blocking

namespace chen
{
    typedef int notifier_t;      // epoll/kqueue handle
    typedef int socket_t;        // socket handle type
    typedef socklen_t option_t;  // socket option size
}

#else

// For Windows
#include <windows.h>
#include <winsock2.h>  // socket defines
#include <ws2tcpip.h>  // getaddrinfo

namespace chen
{
    typedef SOCKET socket_t;  // socket handle type
    typedef int option_t;     // socket option size
}

#endif