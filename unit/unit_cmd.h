/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "unit_base.h"
#include "tool/cmd.h"
#include "tool/log.h"

namespace chen
{
    class unit_cmd : public unit_base
    {
    public:
        virtual void check() override
        {
            chen::cmd flag;

            // test cmd define
            this->check_define(flag);

            // test cmd parse: program command
            this->check_parse_command(flag);

            // test cmd parse: short option
            this->check_parse_short_option(flag);

            // test cmd parse: long option
            this->check_parse_long_option(flag);
        }

        void check_define(chen::cmd &flag)
        {
            PILogD("unit cmd: test define start...");

            try
            {
                flag.define<bool>("bool", "b", "test bool", false, true);
                flag.define<short>("short", "s", "test short", 0, true);
                flag.define<int>("int", "i", "test int", 0, true);
                flag.define<long>("long", "l", "test long");
                flag.define<long long>("long long", "ll", "test long long");
                flag.define<unsigned int>("uint", "ui", "test unsigned int");
                flag.define<unsigned long>("ulong", "ul", "test unsigned long");
                flag.define<unsigned long long>("ulonglong", "ull", "test unsigned long long");
                flag.define<float>("float", "f", "test float");
                flag.define<double>("double", "d", "test double");
                flag.define<long double>("ldouble", "ld", "test long double");
                flag.define<char>("char", "c", "test char");
                flag.define<std::string>("string", "str", "test std::string");
            }
            catch (const std::exception &e)
            {
                PILogF("unit cmd: test define failed, reson: [%s]", e.what());
            }

            PILogD("unit cmd: test define success\n");
        }

        void check_parse_command(chen::cmd &flag)
        {
            PILogD("unit cmd: test command start...");

            // non-exist
            try
            {
                const char *argv[] = {
                        "program",
                        "non-exist"
                };

                flag.parse(2, argv);

                PILogF("unit cmd: test command non-exist failed");
            }
            catch (...)
            {
            }

            // exist
            try
            {
                const char *argv[] = {
                        "program",
                        "bool"
                };

                flag.parse(2, argv);
            }
            catch (...)
            {
                PILogF("unit cmd: test command exist failed");
            }

            PILogD("unit cmd: test command success\n");
        }

        void check_parse_short_option(chen::cmd &flag)
        {
            PILogD("unit cmd: test short option start...");

            // -abc means -a -b -c
            try
            {
                const char *argv[] = {
                        "program",
                        "-bsi"
                };

                flag.parse(2, argv);
            }
            catch (const std::exception &e)
            {
                PILogF("unit cmd: test short option -abc failed, reson: [%s]", e.what());
            }

            // -option argument
            try
            {
                const char *argv[] = {
                        "program",
                        "-string",
                        "test"
                };

                flag.parse(3, argv);

                if (flag.get<std::string>("string") != "test")
                    throw std::runtime_error("test -option argument failed");
            }
            catch (const std::exception &e)
            {
                PILogF("unit cmd: test short option -option argument failed, reson: [%s]", e.what());
            }

            // -option=argument
            try
            {
                const char *argv[] = {
                        "program",
                        "-string=test"
                };

                flag.parse(2, argv);

                if (flag.get<std::string>("string") != "test")
                    throw std::runtime_error("test -option=argument failed");
            }
            catch (const std::exception &e)
            {
                PILogF("unit cmd: test short option -option=argument failed, reson: [%s]", e.what());
            }

            PILogD("unit cmd: test short option success\n");
        }

        void check_parse_long_option(chen::cmd &flag)
        {
            PILogD("unit cmd: test long option start...");

            // --option argument
            try
            {
                const char *argv[] = {
                        "program",
                        "--string",
                        "test"
                };

                flag.parse(3, argv);

                if (flag.get<std::string>("string") != "test")
                    throw std::runtime_error("test --option argument failed");
            }
            catch (const std::exception &e)
            {
                PILogF("unit cmd: test short option --option argument failed, reson: [%s]", e.what());
            }

            // --option=argument
            try
            {
                const char *argv[] = {
                        "program",
                        "--string=test"
                };

                flag.parse(2, argv);

                if (flag.get<std::string>("string") != "test")
                    throw std::runtime_error("test --option=argument failed");
            }
            catch (const std::exception &e)
            {
                PILogF("unit cmd: test short option --option=argument failed, reson: [%s]", e.what());
            }

            PILogD("unit cmd: test long option success\n");
        }

        virtual void bench() override
        {

        }
    };
}