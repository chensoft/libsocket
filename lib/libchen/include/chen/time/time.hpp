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
        virtual ~time() = default;

    public:
        /**
         * Start timing
         */
        virtual void start();

        /**
         * Stop timing
         */
        virtual void stop();

        /**
         * Elapsed time(seconds, in double)
         */
        virtual double elapsed() const;

    protected:
        std::chrono::high_resolution_clock::time_point _beg;
        std::chrono::high_resolution_clock::time_point _end;
    };
}