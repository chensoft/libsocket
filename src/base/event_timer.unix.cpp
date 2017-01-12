/**
 * Created by Jian Chen
 * @since  2017.01.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

#include <socket/base/event_timer.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// timer
chen::event_timer::event_timer()
{

}

chen::event_timer::~event_timer()
{

}

#endif