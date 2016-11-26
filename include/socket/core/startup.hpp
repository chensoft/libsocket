/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

namespace chen
{
    class startup
    {
    public:
        startup();
    };

    // global initialization, force linking the startup code
    static const startup startup_dummy;
}