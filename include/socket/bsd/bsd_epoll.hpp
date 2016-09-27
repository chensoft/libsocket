/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifdef __linux__

namespace chen
{
    namespace bsd
    {
        /**
         * epoll for Linux
         * you should not use this class directly unless you want to implement your own event-based model
         */
        class epoll
        {
            // todo
        };
    }
}

#endif