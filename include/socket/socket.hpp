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
#include "chen/chen.hpp"

// libsocket
#include "socket/config.hpp"

#include "socket/base/basic_address.hpp"
#include "socket/base/basic_option.hpp"
#include "socket/base/basic_socket.hpp"
#include "socket/base/ev_base.hpp"
#include "socket/base/ev_event.hpp"
#include "socket/base/ev_handle.hpp"
#include "socket/base/ev_timer.hpp"

#include "socket/core/ioctl.hpp"
#include "socket/core/reactor.hpp"
#include "socket/core/startup.hpp"

#include "socket/inet/inet_adapter.hpp"
#include "socket/inet/inet_address.hpp"
#include "socket/inet/inet_resolver.hpp"

#include "socket/ip/ip_address.hpp"
#include "socket/ip/ip_option.hpp"
#include "socket/ip/ip_version.hpp"