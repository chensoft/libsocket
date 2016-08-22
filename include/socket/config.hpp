/**
 * Created by Jian Chen
 * @since  2016.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifndef _WIN32

// For Unix-like systems
#include <sys/socket.h>  // socket macros
#include <arpa/inet.h>   // IPv4 and IPv6
#include <unistd.h>      // socket close()
#include <netdb.h>       // getaddrinfo

typedef int socket_t;

#else

// For Windows
#include <windows.h>
#include <winsock2.h>  // socket defines
#include <ws2tcpip.h>  // getaddrinfo

typedef SOCKET socket_t;

#endif