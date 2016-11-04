/**
 * Created by Jian Chen
 * @since  2016.09.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/core/bsd_setup.hpp>
#include <csignal>

// -----------------------------------------------------------------------------
// setup
chen::bsd::setup::setup()
{
    class helper
    {
    public:
        helper()
        {
            // system will trigger SIGPIPE when write to a broken socket
            // the default behaviour for the signal is terminate process
            // since we have error check so we simply ignore this signal
            ::signal(SIGPIPE, SIG_IGN);
        }
    };

    static helper inst;
}

#endif