/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

namespace chen
{
    /**
     * Global initialization
     */
    class startup
    {
    public:
        startup();
    };

    static const startup startup_dummy;  // force linking the startup code
}