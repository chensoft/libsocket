/**
 * libsocket: A Modern C++ Network Library
 * @version   0.1.0
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

#include <socket/sys/epoll.hpp>
#include <socket/sys/kqueue.hpp>
#include <socket/sys/poller.hpp>
#include <socket/sys/runloop.hpp>
#include <socket/sys/startup.hpp>

// todo change
#include <socket/bsd/bsd_endpoint.hpp>
#include <socket/bsd/bsd_option.hpp>
#include <socket/bsd/bsd_socket.hpp>

#include <socket/net/ip/ip_address.hpp>
#include <socket/net/ip/ip_ifaddr.hpp>
#include <socket/net/ip/ip_option.hpp>
#include <socket/net/ip/ip_version.hpp>

#include <socket/net/inet/net_endpoint.hpp>
#include <socket/net/inet/net_resolver.hpp>
#include <socket/net/inet/net_socket.hpp>

#include <socket/net/tcp/tcp_basic.hpp>
#include <socket/net/tcp/tcp_client.hpp>
#include <socket/net/tcp/tcp_conn.hpp>
#include <socket/net/tcp/tcp_event.hpp>
#include <socket/net/tcp/tcp_handler.hpp>
#include <socket/net/tcp/tcp_option.hpp>
#include <socket/net/tcp/tcp_policy.hpp>
#include <socket/net/tcp/tcp_server.hpp>

#include <socket/net/udp/udp_basic.hpp>
#include <socket/net/udp/udp_client.hpp>
#include <socket/net/udp/udp_define.hpp>
#include <socket/net/udp/udp_server.hpp>

#include <socket/net/dns/dns_codec.hpp>
#include <socket/net/dns/dns_define.hpp>
#include <socket/net/dns/dns_edns.hpp>
#include <socket/net/dns/dns_header.hpp>
#include <socket/net/dns/dns_packet.hpp>
#include <socket/net/dns/dns_record.hpp>
#include <socket/net/dns/dns_table.hpp>