/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

namespace chen
{
    class unit_base
    {
    public:
        virtual ~unit_base() = default;

    public:
        /**
         * Testing
         */
        virtual void check() = 0;

        /**
         * Benchmark
         */
        virtual void bench() = 0;
    };
}