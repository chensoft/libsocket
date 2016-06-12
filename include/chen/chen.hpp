/**
 * libchen: A General Purpose C++ Toolkit
 * @version   0.1.0
 * @author    Jian Chen <admin@chensoft.com>
 * @link      http://chensoft.com
 * @license   Licensed under MIT license
 * @copyright Copyright 2016 Jian Chen
 */
#pragma once

// version
constexpr int CHEN_VERSION_MAJOR = 0;
constexpr int CHEN_VERSION_MINOR = 1;
constexpr int CHEN_VERSION_PATCH = 0;

constexpr const char *CHEN_VERSION = "0.1.0";

// include
#include <chen/base/any.hpp>
#include <chen/base/map.hpp>
#include <chen/base/num.hpp>
#include <chen/base/str.hpp>
#include <chen/base/utf8.hpp>
#include <chen/base/array.hpp>
#include <chen/base/regex.hpp>

#include <chen/data/ini.hpp>
#include <chen/data/json.hpp>

#include <chen/mt/callable.hpp>
#include <chen/mt/semaphore.hpp>
#include <chen/mt/threadpool.hpp>

#include <chen/sys/fs.hpp>
#include <chen/sys/sys.hpp>

#include <chen/time/date.hpp>
#include <chen/time/time.hpp>

#include <chen/tool/log.hpp>
#include <chen/tool/cmd.hpp>