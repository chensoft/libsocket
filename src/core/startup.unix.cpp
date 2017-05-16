/**
 * Created by Jian Chen
 * @since  2016.09.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if defined(__unix__) || defined(__APPLE__)

#include "socket/core/startup.hpp"
#include <csignal>

// -----------------------------------------------------------------------------
// startup
chen::startup::startup()
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

    static helper dummy;
}

#endif