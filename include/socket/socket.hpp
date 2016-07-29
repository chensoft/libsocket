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
#include <socket/ip/ip_address.hpp>

#include <socket/so/so_endpoint.hpp>
#include <socket/so/so_socket.hpp>
#include <socket/so/so_error.hpp>

#include <socket/tcp/tcp_define.hpp>
#include <socket/tcp/tcp_socket.hpp>
#include <socket/tcp/tcp_client.hpp>
#include <socket/tcp/tcp_server.hpp>
#include <socket/tcp/tcp_conn.hpp>

#include <socket/udp/udp_define.hpp>
#include <socket/udp/udp_socket.hpp>
#include <socket/udp/udp_client.hpp>
#include <socket/udp/udp_server.hpp>

#include <socket/dns/dns_edns.hpp>
#include <socket/dns/dns_error.hpp>
#include <socket/dns/dns_codec.hpp>
#include <socket/dns/dns_table.hpp>
#include <socket/dns/dns_define.hpp>
#include <socket/dns/dns_record.hpp>
#include <socket/dns/dns_header.hpp>
#include <socket/dns/dns_packet.hpp>