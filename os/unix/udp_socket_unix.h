/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#ifdef CHEN_OS_UNIX

namespace chen
{
    namespace udp
    {
        class socket::impl
        {
        public:
            int _socket = 0;
        };
    }
}

#endif  // CHEN_OS_UNIX