/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

// For global initialization
namespace chen
{
    class setup
    {
    public:
        setup();
    };

    static const setup setup_dummy;  // force linking the setup code
}