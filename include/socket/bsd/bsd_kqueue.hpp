/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#if !defined(__linux__) && !defined(_WIN32)

namespace chen
{
    namespace bsd
    {
        class kqueue
        {

        };
    }
}

#endif