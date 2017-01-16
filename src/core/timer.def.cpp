/**
 * Created by Jian Chen
 * @since  2017.01.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef __linux__

#include <socket/core/timer.hpp>

// -----------------------------------------------------------------------------
// timer
chen::timer::timer()
{

}

chen::timer::~timer()
{

}

void chen::timer::timeout(const std::chrono::nanoseconds &value)
{

}

void chen::timer::interval(const std::chrono::nanoseconds &value)
{

}

void chen::timer::future(const std::chrono::nanoseconds &value)
{

}

void chen::timer::future(const std::chrono::time_point<std::chrono::system_clock> &value)
{

}

chen::basic_handle& chen::timer::handle()
{
    // no use under non-Linux
    static basic_handle dummy;
    return dummy;
}

void chen::timer::update(const std::chrono::time_point<std::chrono::steady_clock> &value)
{

}

void chen::timer::update(const std::chrono::time_point<std::chrono::system_clock> &value)
{

}

#endif