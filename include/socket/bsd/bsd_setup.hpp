/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

// todo if not include in socket.hpp, then still init?

namespace chen
{
    namespace bsd
    {
        class setup
        {
        public:
            /**
             * Global initialization goes here
             */
            setup();
        };

        static const setup setup_dummy;  // force linking the setup code
    }
}