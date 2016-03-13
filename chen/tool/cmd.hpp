/**
 * A modern command line parser
 * @since  2016.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   http://docopt.org
 * -----------------------------------------------------------------------------
 * We accept the following usage:
 * :-) app
 * :-) app -opt1=val --opt2=val ...
 * :-) app action
 * :-) app action -opt1=val --opt2=val ...
 * :-) app action -opt1=val --opt2=val object1 object2 ...
 * :-) app action sub-action -opt1=val --opt2=val object1 object2 ...
 * -----------------------------------------------------------------------------
 * The main three parts is Action + Option + Object
 * Action: the operation that user want to do
 *    e.g: git clone, "clone" is the action
 * Option: the action flags
 *    e.g: git --version, "version" is the flag
 * Object: the unresolved arguments
 *    e.g: git clone https://github.com/chensoft/libchen.git, url is the object
 * Sub-action: an action can contain a sub-action
 *    e.g: git submodule update, "update" is the sub-action of "submodule"
 * -----------------------------------------------------------------------------
 * Usage:
 * >> chen::cmd cmd;
 * >>
 * >> cmd.define("help", "h", "show help", false);
 * >> cmd.define("port", "p", "server port (default: 53)", 53);
 * >> cmd.define("zone", "z", "zone folder", "");
 * >>
 * >> cmd.parse(argc, argv);
 * >>
 * >> bool help = cmd.boolVal("help");
 */
#pragma once

#include "any.hpp"
#include <functional>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace chen
{
    class cmd
    {
    public:
        cmd(const std::string &app = "");
        virtual ~cmd() = default;

    public:
        /**
         * Create an action
         * sub-action's name is separated by dots
         * e.g: git clone, name is "clone"
         * e.g: git submodule init, name is "submodule.init"
         * @param name action name pass a empty string if you don't want a named action
         * @param desc action description
         * @param bind call the function when parse this action
         */
        virtual void create(const std::string &name,
                            const std::string &desc,
                            std::function<void (const chen::cmd &cmd)> bind = nullptr);

        /**
         * Define option of the action
         * @param action identify which action has this option
         * @param name complete name of option, e.g: app --help, "help" is the full name
         * @param tiny short name of option, e.g: app -h, "h" is the short name
         * @param desc description of the option
         * @param def the default value when the option is not specified
         */
        virtual void define(const std::string &name,
                            const std::string &tiny,
                            const std::string &desc,
                            const chen::any &def);

        /**
         * Change the action
         */
        virtual void change(const std::string &name);

    public:
        /**
         * Parse the command line
         */
        virtual void parse(int argc, const char *const argv[]);

    public:
        /**
         * Get the current resolved action name
         */
        virtual std::string current() const;

        /**
         * Get the value of the option which belongs to the current resolved action
         * if the current action doesn't has this option, it will return a default value
         * support bool, int, int64, double, string
         * @param option the full name or tiny name of the option
         */
        virtual bool boolVal(const std::string &name) const;
        virtual std::int32_t intVal(const std::string &name) const;
        virtual std::string strVal(const std::string &name) const;

        virtual std::int64_t int64Val(const std::string &name) const;
        virtual double doubleVal(const std::string &name) const;

        /**
         * Get the object, which is also the unresolved arguments
         * @result the matched params array
         */
        virtual const std::vector<std::string>& object() const;

    public:
        /**
         * App name
         */
        virtual std::string app() const;

        /**
         * Get usage info
         */
        virtual std::string usage() const;

        /**
         * Provide a suggestion for a specific action
         * @param alias action alias
         * @param action action name
         * e.g: app has an action: "remove", if user input "delete", parser will prompt user "Did you mean remove?"
         */
        virtual void suggest(const std::string &alias, const std::string &action);

    protected:
        struct option
        {
            std::string name;
            std::string tiny;
            std::string desc;

            std::string val;
            chen::any   def;

            bool set = false;
        };

        struct action
        {
            std::string name;
            std::string desc;
            std::function<void (const chen::cmd &cmd)> bind;

            std::map<std::string, chen::cmd::option> option;
            std::map<std::string, std::string> alias;
        };

    protected:
        /**
         * Find current action's option
         */
        virtual const struct chen::cmd::option& option(const std::string &name) const;

    protected:
        std::string _app;  // app name

        std::unique_ptr<chen::cmd::action> _action;  // resolved action
        std::vector<std::string> _object;            // unresolved params

        chen::cmd::action *_cursor = nullptr;              // cursor action, weak ref
        std::map<std::string, chen::cmd::action> _define;  // action defines

        std::map<std::string, std::string> _suggest;  // intelligent suggest
    };
}