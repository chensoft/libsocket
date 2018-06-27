/**
 * Created by Jian Chen
 * @since  2016.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

// -----------------------------------------------------------------------------
// Unix-like
#if defined(__unix__) || defined(__APPLE__)

#include <sys/socket.h>   // socket
#include <sys/types.h>    // types

namespace chen
{
    typedef ::ssize_t  ssize_t;  // ssize_t type
    typedef int       handle_t;  // handle type
    typedef socklen_t option_t;  // socket option size

    constexpr handle_t invalid_handle = -1;  // invalid file descriptor
}

#endif


// -----------------------------------------------------------------------------
// Windows
#ifdef _WIN32

namespace chen
{
    typedef long          ssize_t;  // ssize_t type
    typedef unsigned int handle_t;  // handle type
    typedef int          option_t;  // socket option size

    constexpr handle_t invalid_handle = handle_t(~0);  // invalid socket value
}

#endif


// -----------------------------------------------------------------------------
// Android
#ifdef __ANDROID__

// Android support these flags but ndk didn't define them
// unless you compile with android-21 or higher api level
// so we define these macros here to maintain consistency
#ifndef EPOLLONESHOT
#define EPOLLONESHOT 0x40000000
#endif

#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x00002000
#endif

#ifndef EPOLL_CLOEXEC
#define EPOLL_CLOEXEC O_CLOEXEC

// Android lacks the declaration of this function
// epoll_create1 was added to the kernel in 2.6.27
inline int epoll_create1(int flags)
{
    auto fd = ::epoll_create(1);  // 1 is just a hint

    if (flags & EPOLL_CLOEXEC)
        ::ioctl(fd, FIOCLEX);

    return fd;
}

#endif

#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC O_CLOEXEC
#endif

#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK O_NONBLOCK
#endif

#endif


// Global Initialization
#include "socket/core/startup.hpp"