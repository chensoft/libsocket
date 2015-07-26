/**
 * Created by Jian Chen
 * Unified logging mechanism & unlimited output length
 * Prefix "PI" stands for PinIdea, the company I work from 2011.11
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <string>

// macro
#define PILogD(fmt, ...) chen::log::standard().debug(chen::log::format(fmt, ##__VA_ARGS__))
#define PILogE(fmt, ...) chen::log::standard().error(chen::log::format(fmt, ##__VA_ARGS__))
#define PILogF(fmt, ...) chen::log::standard().fatal(chen::log::format(fmt, ##__VA_ARGS__))


// log
namespace chen
{
    class log
    {
    public:
        log();
        virtual ~log();

    public:
        static log& standard();

    public:
        /**
         * Debug trace
         */
        virtual void debug(const std::string &text);

        /**
         * Error trace
         */
        virtual void error(const std::string &text);

        /**
         * Fatal trace
         */
        virtual void fatal(const std::string &text);

        /**
         * Final output
         */
        virtual void flush(const std::string &text);

    public:
        /**
         * Date string
         */
        static std::string date(const std::string &sep = "-", bool gmt = true);

        /**
         * Time string
         */
        static std::string time(const std::string &sep = ":", bool gmt = true, bool milliseconds = false);

        /**
         * Format string using std::vsnprintf
         */
        static std::string format(const char *fmt, ...);
    };
}