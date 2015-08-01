/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 * -----------------------------------------------------------------------------
 * Simple C++ Unit Test Tool
 */
#pragma once

#include <map>
#include <memory>
#include <string>
#include <functional>

namespace chen
{
    class unit_base;

    class unit
    {
    public:
        static unit& instance();

    public:
        /**
         * Testing
         */
        void check();

        /**
         * Benchmark
         */
        void bench();

    private:
        std::map<std::string, std::unique_ptr<unit_base>> _store;

    private:
        unit();
        ~unit();

        /**
         * Prevent the following situation
         * chen::unit &a = chen::unit::instance();
         * chen::unit *b = new chen::unit(a);
         */
        unit(unit const&)           = delete;
        void operator=(unit const&) = delete;
    };
}