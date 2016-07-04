/**
 * Created by Jian Chen
 * @since  2016.07.04
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>

namespace chen
{
    class proc
    {
    public:
        /**
         * Make this app to a daemon
         * after this function return, parent process will exit
         * the current directory will change to '/' on Unix
         * standard file descriptors will be closed
         * @link http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
         * @caution support Unix only
         */
#ifndef _WIN32
        static void daemon();
#endif

        /**
         * Current executable path
         */
        static std::string path(int argc, const char *const argv[]);

        /**
         * Current process id
         * @caution blksize_t, pid_t, and ssize_t shall be signed integer types
         * @link http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_types.h.html
         */
        static int pid();

        /**
         * Kill process
         */
        static bool kill(int pid);

        /**
         * Check if a process exist
         */
        static bool exist(int pid);
    };
}