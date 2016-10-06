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
         * IOCP for Windows(proactor model)
         * you should not use this class directly unless you want to implement your own event-based model
         */
        class iocp
        {
            // todo
        };
    }
}

#endif