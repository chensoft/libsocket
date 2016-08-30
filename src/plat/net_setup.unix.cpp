/**
 * Created by Jian Chen
 * @since  2016.08.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <csignal>

// -----------------------------------------------------------------------------
// setup
namespace
{
    class setup
    {
    public:
        setup()
        {
            ::signal(SIGPIPE, SIG_IGN);
        }
    };

    setup init;  // global initialization
}

#endif