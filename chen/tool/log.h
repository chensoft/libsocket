/**
 * Created by Jian Chen
 * Unified logging mechanism & unlimited output length
 * Prefix "PI" stands for PinIdea, the company I work from 2011.11
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <cstdlib>
#include <mutex>
#include "str.h"

// macro
#define PILogD(FORMAT, ...) (chen::log::standard().debug(FORMAT, ##__VA_ARGS__))
#define PILogE(FORMAT, ...) (chen::log::standard().error(FORMAT, ##__VA_ARGS__))
#define PILogF(FORMAT, ...) (chen::log::standard().fatal(FORMAT, ##__VA_ARGS__))


// log
namespace chen
{
    class log
    {
    public:
        static log& standard();

    public:
        log() = default;
        virtual ~log() = default;

    public:
        /**
         * Debug trace
         */
        template <typename ... Args>
        void debug(const char *format, Args ... args);

        /**
         * Error trace
         */
        template <typename ... Args>
        void error(const char *format, Args ... args);

        /**
         * Fatal trace
         */
        template <typename ... Args>
        void fatal(const char *format, Args ... args);

        /**
         * Final output
         */
        virtual void flush(const std::string &text);

    protected:
        static std::mutex _mutex;
    };
}


namespace chen
{
    /**
     * Log
     */
    template <typename ... Args>
    void log::debug(const char *format, Args ... args)
    {
        this->flush("[DEBUG] " + chen::str::format(format, args...));
    }

    template <typename ... Args>
    void log::error(const char *format, Args ... args)
    {
        this->flush("[ERROR] " + chen::str::format(format, args...));
    }

    template <typename ... Args>
    void log::fatal(const char *format, Args ... args)
    {
        this->flush("[FATAL] " + chen::str::format(format, args...));

        std::exit(EXIT_FAILURE);
    }
}