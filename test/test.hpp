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
    class test_base;

    class test final
    {
    public:
        static test& instance();

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
        std::map<std::string, std::unique_ptr<test_base>> _store;

    private:
        test();
        ~test() = default;

        /**
         * Prevent the following situation
         * chen::test &a = chen::test::instance();
         * chen::test *b = new chen::test(a);
         */
        test(const test &) = delete;
        test& operator=(const test &) = delete;
    };
}