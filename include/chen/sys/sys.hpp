/**
 * Created by Jian Chen
 * @since  2016.01.26
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>
#include <vector>

namespace chen
{
    class sys
    {
    public:
        /**
         * Make this app to a daemon
         * after this function return, parent process will exit
         * the current directory will change to '/' on Unix
         * standard file descriptors will be closed
         * @link http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
         */
        static void daemon();

        /**
         * UUID string
         */
        static std::string uuid();
    };
}