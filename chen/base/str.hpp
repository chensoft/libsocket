/**
 * Created by Jian Chen
 * @since  2015.07.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <ostream>
#include <string>

namespace chen
{
    class str
    {
    public:
        /**
         * Format string using std::vsnprintf
         */
        static std::string format(const char *fmt, ...);

        /**
         * Print string to standard output
         * thread safe, guarantee the output order
         */
        template <typename ...Args>
        static void printf(const char *fmt, Args... args)
        {
            chen::str::print(chen::str::format(fmt, args...), false);
        }

        template <typename ...Args>
        static void printf(std::ostream &out, const char *fmt, Args... args)
        {
            chen::str::print(out, chen::str::format(fmt, args...), false);
        }

        static void print(const std::string &text, bool end = true);
        static void print(std::ostream &out, const std::string &text, bool end = true);

        /**
         * Test if two strings are equal
         */
        static bool equal(const char *str1, std::size_t size1,
                          const char *str2, std::size_t size2);

        /**
         * Trim white spaces
         */
        static void trim(std::string &text);
        static void ltrim(std::string &text);
        static void rtrim(std::string &text);

        /**
         * Levenshtein distance
         * @link https://en.wikipedia.org/wiki/Levenshtein_distance
         */
        static std::size_t levenshtein(const char *s, std::size_t len_s, const char *t, std::size_t len_t);
    };
}