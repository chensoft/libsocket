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

#include <socket/bsd/bsd_endpoint.hpp>
#include <socket/bsd/bsd_epoll.hpp>
#include <socket/bsd/bsd_kqueue.hpp>
#include <socket/bsd/bsd_option.hpp>
#include <socket/bsd/bsd_poller.hpp>
#include <socket/bsd/bsd_setup.hpp>
#include <socket/bsd/bsd_socket.hpp>

#include <socket/prot/ip/ip_address.hpp>
#include <socket/prot/ip/ip_ifaddr.hpp>
#include <socket/prot/ip/ip_option.hpp>
#include <socket/prot/ip/ip_version.hpp>

#include <socket/prot/inet/net_endpoint.hpp>
#include <socket/prot/inet/net_resolver.hpp>
#include <socket/prot/inet/net_runloop.hpp>
#include <socket/prot/inet/net_socket.hpp>

#include <socket/prot/tcp/tcp_basic.hpp>
#include <socket/prot/tcp/tcp_client.hpp>
#include <socket/prot/tcp/tcp_conn.hpp>
#include <socket/prot/tcp/tcp_event.hpp>
#include <socket/prot/tcp/tcp_handler.hpp>
#include <socket/prot/tcp/tcp_option.hpp>
#include <socket/prot/tcp/tcp_policy.hpp>
#include <socket/prot/tcp/tcp_server.hpp>

#include <socket/prot/udp/udp_basic.hpp>
#include <socket/prot/udp/udp_client.hpp>
#include <socket/prot/udp/udp_define.hpp>
#include <socket/prot/udp/udp_server.hpp>

#include <socket/prot/dns/dns_codec.hpp>
#include <socket/prot/dns/dns_define.hpp>
#include <socket/prot/dns/dns_edns.hpp>
#include <socket/prot/dns/dns_header.hpp>
#include <socket/prot/dns/dns_packet.hpp>
#include <socket/prot/dns/dns_record.hpp>
#include <socket/prot/dns/dns_table.hpp>