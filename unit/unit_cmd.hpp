/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "unit_base.hpp"
#include <chen/tool/cmd.hpp>
#include <chen/tool/log.hpp>

namespace chen
{
    class unit_cmd : public unit_base
    {
    public:
//        virtual void check() override
//        {
//            chen::cmd flag;
//
//            // test cmd define
//            this->check_define(flag);
//
//            // test cmd parse: program command
//            this->check_parse_command(flag);
//
//            // test cmd parse: short option
//            this->check_parse_short_option(flag);
//
//            // test cmd parse: long option
//            this->check_parse_long_option(flag);
//        }
//
//        void check_define(chen::cmd &flag)
//        {
//            PILogD("unit cmd: test define start...");
//
//            try
//            {
//                flag.define<bool>("bool1", "b", false, "test bool 1");
//                flag.define<bool>("bool2", "z", false, "test bool 2");
//                flag.define<short>("short", "s", 0, "test short");
//                flag.define<int>("int", "i", 0, "test int");
//                flag.define<long>("long", "l", 0, "test long");
//                flag.define<long long>("long long", "ll", 0, "test long long");
//                flag.define<unsigned int>("uint", "ui", 0, "test unsigned int");
//                flag.define<unsigned long>("ulong", "ul", 0, "test unsigned long");
//                flag.define<unsigned long long>("ulonglong", "ull", 0, "test unsigned long long");
//                flag.define<float>("float", "f", 0.0, "test float");
//                flag.define<double>("double", "d", 0.0, "test double");
//                flag.define<long double>("ldouble", "ld", 0.0, "test long double");
//                flag.define<char>("char", "c", '\0', "test char");
//                flag.define<std::string>("string", "str", "", "test std::string");
//            }
//            catch (const std::exception &e)
//            {
//                PILogF("unit cmd: test define failed, reson: [%s]", e.what());
//            }
//
//            PILogD("unit cmd: test define success\n");
//        }
//
//        void check_parse_command(chen::cmd &flag)
//        {
//            PILogD("unit cmd: test command start...");
//
//            // non-exist
//            try
//            {
//                const char *argv[] = {
//                        "program",
//                        "non-exist"
//                };
//
//                flag.parse(2, argv);
//
//                PILogF("unit cmd: test command non-exist failed");
//            }
//            catch (...)
//            {
//            }
//
//            // exist
//            try
//            {
//                const char *argv[] = {
//                        "program",
//                        "bool1"
//                };
//
//                flag.parse(2, argv);
//            }
//            catch (...)
//            {
//                PILogF("unit cmd: test command exist failed");
//            }
//
//            PILogD("unit cmd: test command success\n");
//        }
//
//        void check_parse_short_option(chen::cmd &flag)
//        {
//            PILogD("unit cmd: test short option start...");
//
//            // -abc means -a -b -c
//            try
//            {
//                const char *argv[] = {
//                        "program",
//                        "-bz"
//                };
//
//                flag.parse(2, argv);
//            }
//            catch (const std::exception &e)
//            {
//                PILogF("unit cmd: test short option -abc failed, reson: [%s]", e.what());
//            }
//
//            // -option argument
//            try
//            {
//                const char *argv[] = {
//                        "program",
//                        "-string",
//                        "test"
//                };
//
//                flag.parse(3, argv);
//
//                if (flag.get<std::string>("string") != "test")
//                    throw std::runtime_error("test: -option argument failed");
//            }
//            catch (const std::exception &e)
//            {
//                PILogF("unit cmd: test short option -option argument failed, reson: [%s]", e.what());
//            }
//
//            // -option=argument
//            try
//            {
//                const char *argv[] = {
//                        "program",
//                        "-string=test"
//                };
//
//                flag.parse(2, argv);
//
//                if (flag.get<std::string>("string") != "test")
//                    throw std::runtime_error("test: -option=argument failed");
//            }
//            catch (const std::exception &e)
//            {
//                PILogF("unit cmd: test short option -option=argument failed, reson: [%s]", e.what());
//            }
//
//            PILogD("unit cmd: test short option success\n");
//        }
//
//        void check_parse_long_option(chen::cmd &flag)
//        {
//            PILogD("unit cmd: test long option start...");
//
//            // --option argument
//            try
//            {
//                const char *argv[] = {
//                        "program",
//                        "--string",
//                        "test"
//                };
//
//                flag.parse(3, argv);
//
//                if (flag.get<std::string>("string") != "test")
//                    throw std::runtime_error("test: --option argument failed");
//            }
//            catch (const std::exception &e)
//            {
//                PILogF("unit cmd: test short option --option argument failed, reson: [%s]", e.what());
//            }
//
//            // --option=argument
//            try
//            {
//                const char *argv[] = {
//                        "program",
//                        "--string=test"
//                };
//
//                flag.parse(2, argv);
//
//                if (flag.get<std::string>("string") != "test")
//                    throw std::runtime_error("test: --option=argument failed");
//            }
//            catch (const std::exception &e)
//            {
//                PILogF("unit cmd: test short option --option=argument failed, reson: [%s]", e.what());
//            }
//
//            PILogD("unit cmd: test long option success\n");
//        }

        virtual void bench() override
        {

        }
    };
}