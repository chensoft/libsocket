/**
 * Created by Jian Chen
 * @since  2016.08.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/net/net_config.hpp>
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
chen::net::setup::setup()
{
    // global initialization
    static helper inst;
}

#endif