/**
 * Created by Jian Chen
 * Unified logging mechanism & unlimited output length
 * Prefix "PI" stands for PinIdea, the company I work from 2011.11
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   https://github.com/nlog/nlog/wiki/Log-levels
 */
#pragma once

#include <chen/base/str.hpp>
#include <functional>
#include <cstdlib>
#include <mutex>

// log
namespace chen
{
    class log
    {
    public:
        static log& standard();

    public:
        enum class Level {Trace, Debug, Info, Warn, Error, Fatal};

    public:
        log() = default;
        virtual ~log() = default;

    public:
        /**
         * Trace
         * e.g: begin method X, end method X etc
         */
        template <typename ...Args>
        void trace(const char *format, Args... args)
        {
            if (this->_level <= Level::Trace)
                this->output(chen::str::format(format, args...), Level::Trace);
        }

        /**
         * Debug
         * e.g: executed queries, user authenticated, session expired
         */
        template <typename ...Args>
        void debug(const char *format, Args... args)
        {
            if (this->_level <= Level::Debug)
                this->output(chen::str::format(format, args...), Level::Debug);
        }

        /**
         * Info
         * e.g: normal behavior like mail sent, user updated profile etc
         */
        template <typename ...Args>
        void info(const char *format, Args... args)
        {
            if (this->_level <= Level::Info)
                this->output(chen::str::format(format, args...), Level::Info);
        }

        /**
         * Warn
         * e.g: incorrect behavior but the application can continue
         */
        template <typename ...Args>
        void warn(const char *format, Args... args)
        {
            if (this->_level <= Level::Warn)
                this->output(chen::str::format(format, args...), Level::Warn);
        }

        /**
         * Error
         * e.g: for example application crashes / exceptions
         */
        template <typename ...Args>
        void error(const char *format, Args... args)
        {
            if (this->_level <= Level::Error)
                this->output(chen::str::format(format, args...), Level::Error);
        }

        /**
         * Fatal
         * e.g: highest level: important stuff down
         */
        template <typename ...Args>
        void fatal(const char *format, Args... args)
        {
            if (this->_level <= Level::Fatal)
            {
                this->output(chen::str::format(format, args...), Level::Fatal);
                std::exit(EXIT_FAILURE);
            }
        }

    public:
        /**
         * Limit log level
         * log below this level will not be output
         */
        virtual void limit(Level level);

        /**
         * Get log level
         */
        virtual Level level() const;

    public:
        /**
         * Hook the output
         * user shall not call log's method again in callback, otherwise will deadlock
         */
        virtual void hook(std::function<void (std::string &&text)> callback);

    protected:
        /**
         * Final output
         */
        virtual void output(std::string &&text, chen::log::Level level);

    private:
        log(const log&) = delete;
        log& operator=(const log&) = delete;

    protected:
        Level _level = Level::Trace;

        std::mutex _mutex;
        std::function<void (std::string &&text)> _hook;
    };
}

// helper
template <typename ...Args>
inline void PILogT(const char *format, Args... args)
{
    chen::log::standard().trace(format, args...);
}

template <typename ...Args>
inline void PILogD(const char *format, Args... args)
{
    chen::log::standard().debug(format, args...);
}

template <typename ...Args>
inline void PILogI(const char *format, Args... args)
{
    chen::log::standard().info(format, args...);
}

template <typename ...Args>
inline void PILogW(const char *format, Args... args)
{
    chen::log::standard().warn(format, args...);
}

template <typename ...Args>
inline void PILogE(const char *format, Args... args)
{
    chen::log::standard().error(format, args...);
}

template <typename ...Args>
inline void PILogF(const char *format, Args... args)
{
    chen::log::standard().fatal(format, args...);
}