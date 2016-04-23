/**
 * Created by Jian Chen
 * @since  2015.08.04
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "routine.hpp"

using namespace chen;

// ------------------------------------------------------------------
// routine
routine::routine(routine &&o)
{
    *this = std::move(o);
}

routine& routine::operator=(routine &&o)
{
    if (this == &o)
        return *this;

    return *this;
}

void routine::async(const std::function<void ()> &job)
{
    // no implement now
}