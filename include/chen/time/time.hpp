/**
 * Created by Jian Chen
 * @since  2016.04.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chrono>

namespace chen
{
    class time
    {
    public:
        explicit time(bool start = false);

    public:
        /**
         * Start timing
         */
        void start();

        /**
         * Stop timing
         */
        void stop();

        /**
         * Elapsed time(seconds, in double)
         */
        double elapsed() const;

    protected:
        std::chrono::high_resolution_clock::time_point _beg;
        std::chrono::high_resolution_clock::time_point _end;
    };
}