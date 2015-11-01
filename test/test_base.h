/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <chen/tool/log.h>

namespace chen
{
    class test_base
    {
    public:
        virtual ~test_base() = default;

    public:
        /**
         * Testing
         */
        virtual void check() = 0;

        /**
         * Benchmark
         */
        virtual void bench() = 0;

    public:
        template <typename ... Args>
        void assert(bool condition, const char *format, Args ... args)
        {
            if (!condition)
                PILogF(format, args...);
        }
    };
}