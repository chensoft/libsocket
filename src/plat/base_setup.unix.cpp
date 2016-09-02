/**
 * Created by Jian Chen
 * @since  2016.08.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/base/base_setup.hpp>
#include <csignal>

// -----------------------------------------------------------------------------
// helper
namespace
{
    class helper
    {
    public:
        helper()
        {
            ::signal(SIGPIPE, SIG_IGN);
        }
    };
}


// -----------------------------------------------------------------------------
// setup
chen::setup::setup()
{
    // global initialization
    static helper inst;
}

#endif