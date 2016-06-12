/**
 * Created by Jian Chen
 * @since  2015.07.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <ostream>
#include <vector>
#include <string>
#include <mutex>

namespace chen
{
    class str
    {
    public:
        /**
         * Format string using std::vsnprintf
         * @thread-safe
         */
        static std::string format(const char *fmt)
        {
            // to improve performance, return fmt if no other params
            return fmt;
        }

        template <typename ...Args>
        static std::string format(const char *fmt, Args... args)
        {
            auto size = ::snprintf(nullptr, 0, fmt, args...);
            if (size <= 0)
                return "";

            std::size_t len = static_cast<std::size_t>(size + 1);
            std::string ret(len, '\0');

            ::snprintf(&ret[0], len, fmt, args...);

            ret.erase(ret.begin() + size);  // remove the redundant '\0'

            return ret;
        }

        /**
         * Print string to standard output or custom stream, the output order is guaranteed
         * @thread-safe
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

        template <typename ...Args>
        static void printf(std::ostream &out, std::mutex &mutex, const char *fmt, Args... args)
        {
            chen::str::print(out, mutex, chen::str::format(fmt, args...), false);
        }

        static void print(const std::string &text, bool br = true);
        static void print(std::ostream &out, const std::string &text, bool br = true);
        static void print(std::ostream &out, std::mutex &mutex, const std::string &text, bool br = true);

        /**
         * Test if two strings are equal
         */
        static bool equal(const char *str1, std::size_t size1,
                          const char *str2, std::size_t size2);

        /**
         * Check prefix & suffix
         */
        static bool prefix(const std::string &text, const std::string &prefix);
        static bool suffix(const std::string &text, const std::string &suffix);

        /**
         * Test if text contains a string
         */
        static bool contain(const std::string &text, const std::string &search);

        /**
         * Check the number of occurrences of search in the string
         */
        static std::size_t count(const std::string &text, const std::string &search);

        /**
         * Split a string to vector
         */
        static std::vector<std::string> split(const std::string &text, const std::string &delimiter);

        /**
         * Join a string vector to single string
         */
        static std::string join(const std::vector<std::string> &text, const std::string &delimiter);

        /**
         * Replace text
         */
        static std::string& replace(std::string &text,
                                    const std::string &search,
                                    const std::string &replace,
                                    bool all = true);
        static std::string& remove(std::string &text,
                                   const std::string &search,
                                   bool all = true);

        /**
         * Trim white spaces
         */
        static std::string& trim(std::string &text);
        static std::string& ltrim(std::string &text);
        static std::string& rtrim(std::string &text);

        /**
         * Levenshtein distance
         * @link https://en.wikipedia.org/wiki/Levenshtein_distance
         */
        static std::size_t levenshtein(const char *s, std::size_t len_s, const char *t, std::size_t len_t);
    };
}