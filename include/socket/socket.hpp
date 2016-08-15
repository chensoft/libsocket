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
#include <socket/net/net_interface.hpp>
#include <socket/net/net_endpoint.hpp>
#include <socket/net/net_version.hpp>
#include <socket/net/net_address.hpp>
#include <socket/net/net_socket.hpp>
#include <socket/net/net_error.hpp>

#include <socket/dns/dns_define.hpp>
#include <socket/dns/dns_header.hpp>
#include <socket/dns/dns_packet.hpp>
#include <socket/dns/dns_record.hpp>
#include <socket/dns/dns_codec.hpp>
#include <socket/dns/dns_table.hpp>
#include <socket/dns/dns_error.hpp>
#include <socket/dns/dns_edns.hpp>