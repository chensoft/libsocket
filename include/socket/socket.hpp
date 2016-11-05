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

//#include <socket/sys/epoll.hpp>
//#include <socket/sys/kqueue.hpp>
//#include <socket/sys/poller.hpp>
//#include <socket/sys/runloop.hpp>
//#include <socket/sys/startup.hpp>
//
//#include <socket/bsd/basic_endpoint.hpp>
//#include <socket/bsd/basic_option.hpp>
//#include <socket/bsd/basic_socket.hpp>
//
//#include <socket/ip/ip_address.hpp>
//#include <socket/ip/ip_ifaddr.hpp>
//#include <socket/ip/ip_option.hpp>
//#include <socket/ip/ip_version.hpp>
//
//#include <socket/inet/inet_endpoint.hpp>
//#include <socket/inet/inet_resolver.hpp>
//#include <socket/inet/inet_socket.hpp>
//
//#include <socket/tcp/tcp_basic.hpp>
//#include <socket/tcp/tcp_client.hpp>
//#include <socket/tcp/tcp_conn.hpp>
//#include <socket/tcp/tcp_event.hpp>
//#include <socket/tcp/tcp_handler.hpp>
//#include <socket/tcp/tcp_option.hpp>
//#include <socket/tcp/tcp_policy.hpp>
//#include <socket/tcp/tcp_server.hpp>
//
//#include <socket/udp/udp_basic.hpp>
//#include <socket/udp/udp_client.hpp>
//#include <socket/udp/udp_define.hpp>
//#include <socket/udp/udp_server.hpp>