/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/ev_base.hpp>

namespace chen
{
    /**
     * The base class that has a file descriptor
     */
    class ev_handle: public ev_base
    {
    public:
    };
}