/**
 * A modern command line parser
 * @since  2016.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   http://docopt.org
 * -----------------------------------------------------------------------------
 * We accept the following usage format:
 * :-) app
 * :-) app -opt1=val --opt2=val ...
 * :-) app action
 * :-) app action -opt1=val --opt2=val ...
 * :-) app action object1 object2 -opt1=val --opt2=val ...
 * :-) app action sub-action object1 object2 -opt1=val --opt2=val ...
 * -----------------------------------------------------------------------------
 * The main three parts is Action + Object + Option
 * Action: the operation that user want to do
 *    e.g: git clone, "clone" is the action
 * Object: the operation target
 *    e.g: git clone https://github.com/chensoft/libchen.git, url is the object
 * Option: the action flags
 *    e.g: git --version, "version" is the flag
 * Sub-action: an action can contain a sub-action
 *    e.g: git submodule update, "update" is the sub-action of "submodule"
 * -----------------------------------------------------------------------------
 * Usage: todo
 * >> chen::cmd flag;
 * >>
 * >> flag.define<bool>("help", "h", "show help");
 * >> flag.define<int>("port", "p", "server port (default: 53)", 53);
 * >> flag.define<std::string>("zone", "z", "zone folder", "");
 * >>
 * >> flag.parse(argc, argv);
 * >>
 * >> bool help = flag.get<bool>("help");
 */
#pragma once

#include "str.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

namespace chen
{
    // -------------------------------------------------------------------------
    // cmd
    class cmd
    {
    public:
        cmd(const std::string &app = "");
        virtual ~cmd() = default;

    protected:
        // action + object + option
        class action;
        class object;
        class option;
        class error;

        // predicate
        template <typename T>
        class one_of_helper;

        template <typename T>
        class range_of_helper;

    public:
        /**
         * Parse command line
         */
        virtual void parse(int argc, const char *const argv[]);

        /**
         * Define action, sub-action's name is separated by dots
         * e.g: git clone, name is "action"
         * e.g: git submodule init, name is "submodule.init"
         */
        virtual void action(const std::string &name,
                            const std::string &desc,
                            std::function<void (chen::cmd &cmd)> bind = nullptr);

        /**
         * Define object
         * @param action identify which action has this object
         * @param limit the max count of object
         */
        virtual void object(const std::string &action, int limit);

        /**
         * Define option
         * @param action identify which action has this option
         * @param full complete name of option, e.g: app --help, "help" is the full name
         * @param tiny short name of option, e.g: app -h, "h" is the short name
         * @param desc description of the option
         * @param def the default value when the option is not specified
         */
        template <typename T>
        void option(const std::string &action,
                    const std::string &full,
                    const std::string &tiny,
                    const std::string &desc,
                    const T &def = T())
        {

        }

    public:
        /**
         * Get current matched action name
         */
        virtual std::string action() const;

        /**
         * Rest unresolved arguments
         */
        virtual const std::vector<std::string>& rest() const;

        /**
         * Get the value of the option
         * if the current action doesn't has this option, an error will be thrown
         * support bool, int, int64, double, string, it's enough to get value about cli
         * @param option complete or short name of the option
         */
        virtual bool boolVal(const std::string &option) const;
        virtual int intVal(const std::string &option) const;
        virtual std::string strVal(const std::string &option) const;

        virtual long long int64Val(const std::string &option) const;
        virtual double doubleVal(const std::string &option) const;

    public:
        /**
         * Show usage info
         */
        virtual void usage();

        /**
         * The text before the usage body
         */
        virtual void prefix(const std::string &text);

        /**
         * The text after the usage body
         */
        virtual void suffix(const std::string &text);

    protected:
        /**
         * Print usage info
         */
        template <typename ... Args>
        void print(const char *format, Args ... args)
        {
            this->output(str::format(format, args...));
        }

        /**
         * Final output
         */
        virtual void output(const std::string &text);

    public:
        /**
         * Predicate
         */
        template <typename T, typename ... Args>
        static one_of_helper<T> one_of(Args ... args)
        {
            return one_of_helper<T>({args...});
        }

        template <typename T>
        static range_of_helper<T> range_of(T beg, T end)
        {
            return range_of_helper<T>(beg, end);
        }

    protected:
        std::string _app;
        std::string _prefix;
        std::string _suffix;

        std::vector<std::string> _rest;
    };


    // -------------------------------------------------------------------------
    // action
    class cmd::action
    {

    };


    // -------------------------------------------------------------------------
    // object
    class cmd::object
    {

    };


    // -------------------------------------------------------------------------
    // option
    class cmd::option
    {

    };


    // -------------------------------------------------------------------------
    // error
    class cmd::error : public std::runtime_error
    {
    public:
        explicit error(const std::string &what) : std::runtime_error(what) {}
    };


    // -------------------------------------------------------------------------
    // one_of_helper, used for string or number
    template <typename T>
    class cmd::one_of_helper
    {
    public:
        one_of_helper(std::initializer_list<T> init)
                : _data(init)
        {

        }

        bool operator()(const std::string &val)
        {
            T dst;

            std::istringstream ss(val);
            ss >> dst;

            return this->_data.find(dst) != this->_data.end();
        }

    private:
        std::set<T> _data;
    };


    // -------------------------------------------------------------------------
    // range_of_helper, [beg, end], mainly used for number
    template <typename T>
    class cmd::range_of_helper
    {
    public:
        range_of_helper(T beg, T end)
        : _beg(beg)
        , _end(end)
        {

        }

        bool operator()(const std::string &val)
        {
            T dst;

            std::istringstream ss(val);
            ss >> dst;

            return (dst >= this->_beg) && (dst <= this->_end);
        }

    private:
        T _beg;
        T _end;
    };
}