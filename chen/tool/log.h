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

// todo ad log level enum, and user can change log mode
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
        void debug(const char *format, Args ... args)
        {
            this->flush("[DEBUG] " + chen::str::format(format, args...));
        }

        /**
         * Error trace
         */
        template <typename ... Args>
        void error(const char *format, Args ... args)
        {
            this->flush("[ERROR] " + chen::str::format(format, args...));
        }

        /**
         * Fatal trace
         */
        template <typename ... Args>
        void fatal(const char *format, Args ... args)
        {
            this->flush("[FATAL] " + chen::str::format(format, args...));
            std::exit(EXIT_FAILURE);
        }

        /**
         * Final output
         */
        virtual void flush(const std::string &text);

    protected:
        static std::mutex _mutex;
    };
}

// helper
template <typename ... Args>
inline void PILogD(const char *format, Args ... args)
{
    chen::log::standard().debug(format, args...);
}

template <typename ... Args>
inline void PILogE(const char *format, Args ... args)
{
    chen::log::standard().error(format, args...);
}

template <typename ... Args>
inline void PILogF(const char *format, Args ... args)
{
    chen::log::standard().fatal(format, args...);
}