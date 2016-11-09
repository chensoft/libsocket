/**
 * libsocket: A Modern C++ Network Library
 * @author    Jian Chen <admin@chensoft.com>
 * @link      http://chensoft.com
 * @license   Licensed under MIT license
 * @copyright Copyright 2016 Jian Chen
 */
#pragma once

// version
constexpr int SOCKET_VERSION_MAJOR = 0;
constexpr int SOCKET_VERSION_MINOR = 1;
constexpr int SOCKET_VERSION_PATCH = 0;

constexpr const char *SOCKET_VERSION = "0.1.0";

// libchen
#include <chen/chen.hpp>

// libsocket
#include <socket/config.hpp>

#include <socket/core/epoll.hpp>
#include <socket/core/kqueue.hpp>
#include <socket/core/poller.hpp>
#include <socket/core/runloop.hpp>
#include <socket/core/startup.hpp>

#include <socket/base/bsd_address.hpp>
#include <socket/base/bsd_option.hpp>
#include <socket/base/bsd_socket.hpp>

#include <socket/base/dgram_client.hpp>
#include <socket/base/dgram_server.hpp>
#include <socket/base/dgram_socket.hpp>

#include <socket/ip/ip_address.hpp>
#include <socket/ip/ip_ifaddr.hpp>
#include <socket/ip/ip_option.hpp>
#include <socket/ip/ip_version.hpp>

#include <socket/inet/inet_address.hpp>
#include <socket/inet/inet_resolver.hpp>

//#include <socket/tcp/tcp_basic.hpp>
//#include <socket/tcp/tcp_client.hpp>
//#include <socket/tcp/tcp_conn.hpp>
//#include <socket/tcp/tcp_event.hpp>
//#include <socket/tcp/tcp_handler.hpp>
//#include <socket/tcp/tcp_option.hpp>
//#include <socket/tcp/tcp_policy.hpp>
//#include <socket/tcp/tcp_server.hpp>

#include <socket/udp/udp_client.hpp>
#include <socket/udp/udp_define.hpp>
#include <socket/udp/udp_option.hpp>
#include <socket/udp/udp_server.hpp>