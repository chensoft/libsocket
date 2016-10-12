/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifdef _WIN32

namespace chen
{
    namespace bsd
    {
        /**
         * WSAPoll for Windows(reactor model)
         * Windows's most efficient model is IOCP, but IOCP is a proactor model, so we use WSAPoll here
         * you should not use this class directly unless you want to implement your own event-based model
         */
        class poller
        {
            // todo
        };


        /**
         * reactor typedef
         */
        typedef kqueue reactor;
    }
}

#endif