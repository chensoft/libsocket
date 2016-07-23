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
 * :-) app action ...
 * :-) app action -opt1=val --opt2=val ...
 * :-) app action -opt1=val --opt2=val object1 object2 ...
 * :-) app action sub-action -opt1=val --opt2=val object1 object2 ...
 * -----------------------------------------------------------------------------
 * The core concepts are Action + Option + Object
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
 * >> try
 * >> {
 * >>     cmd.define("help", "h", "show help", false);
 * >>     cmd.define("port", "p", "server port (default: 53)", 53);
 * >>     cmd.define("zone", "z", "zone folder", "");
 * >>
 * >>     cmd.parse(argc, argv);
 * >>
 * >>     bool help = cmd.boolVal("help");
 * >> }
 * >> catch (const chen::cmd::error_parse &e)
 * >> {
 * >>     std::cout << cmd.usage(e) << std::endl;
 * >> }
 * >> catch (const std::exception &e)
 * >> {
 * >>     std::cout << e.what() << std::endl;
 * >> }
 */
#pragma once

#include <chen/base/any.hpp>
#include <chen/base/str.hpp>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>

namespace chen
{
    class cmd
    {
    public:
        explicit cmd(const std::string &app = "");

    public:
        /**
         * Create an action
         * sub-action's name is separated by dots
         * e.g: git clone, name is "clone"
         * e.g: git submodule init, name is "submodule.init"
         * @param action the action name
         * @param desc action description
         * @param bind call the function when parse this action
         */
        void create(const std::string &action,
                    const std::string &desc,
                    std::function<void (const chen::cmd &cmd)> bind = nullptr);

        /**
         * Define option of the action
         * @param option complete name of option, e.g: app --help, "help" is the full name
         * @param tiny short name of option, e.g: app -h, "h" is the short name
         * @param desc description of the option
         * @param def the default value when the option is not specified
         */
        void define(const std::string &option,
                    const std::string &tiny,
                    const std::string &desc,
                    const chen::any &def);

        /**
         * Change the action
         */
        void change(const std::string &action);

        /**
         * Check if the action or option exist
         */
        bool exist(const std::string &action, const std::string &option = "") const;

    public:
        /**
         * Parse the command line
         */
        void parse(int argc, const char *const argv[]);

    public:
        /**
         * App name
         */
        std::string app() const;

        /**
         * Current resolved action name
         */
        std::string current() const;

        /**
         * Get the value of the option which belongs to the current resolved action
         * support bool, int, string
         * @param option the full name or tiny name of the option
         */
        bool boolVal(const std::string &option) const;
        std::int32_t intVal(const std::string &option) const;
        std::string strVal(const std::string &option) const;

        /**
         * Whether the option is set
         */
        bool isSet(const std::string &option) const;

        /**
         * Get the objects, which is also the unresolved arguments
         */
        const std::vector<std::string>& objects() const;

    public:
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

            std::vector<std::string> order;  // option insertion order
            std::unordered_map<std::string, chen::cmd::option> options;
            std::unordered_map<std::string, std::string> alias;
        };

        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : std::runtime_error(what) {}
        };

        class error_general : public chen::cmd::error
        {
        public:
            explicit error_general(const std::string &what) : chen::cmd::error(what) {}
        };

        class error_parse : public chen::cmd::error
        {
        public:
            explicit error_parse(const std::string &what,
                                 const std::string &action,
                                 const std::string &option)
                    : chen::cmd::error(what), action(action), option(option) {}

        public:
            std::string action;
            std::string option;
        };

    public:
        /**
         * Get usage info
         */
        std::string usage() const;
        std::string usage(const std::string &action) const;
        std::string usage(const std::string &action, const std::string &option) const;
        std::string usage(const chen::cmd::error_parse &error) const;

        /**
         * Visit actions and options
         * empty name means it's the root action
         */
        void visit(std::function<void (const chen::cmd::action &action, std::size_t idx, std::size_t len)> callback,
                   std::function<bool (const std::string &a, const std::string &b)> compare = nullptr) const;
        void visit(const std::string &action,
                   std::function<void (const chen::cmd::option &option, std::size_t idx, std::size_t len)> callback,
                   std::function<bool (const std::string &a, const std::string &b)> compare = nullptr) const;

        /**
         * Provide a suggestion for a specific action
         * @param alias action alias
         * @param action action name
         * e.g: app has an action: "remove", if user input "delete", parser will prompt user "Did you mean remove?"
         */
        void suggest(const std::string &alias, const std::string &action);

    protected:
        /**
         * Find current action's option
         */
        const chen::cmd::option& opt(const std::string &name) const;

    protected:
        std::string _app;  // app name

        std::unique_ptr<chen::cmd::action> _action;  // resolved action
        std::vector<std::string> _objects;           // unresolved params

        chen::cmd::action *_cursor = nullptr;              // cursor action, weak ref

        std::vector<std::string> _order;  // action insertion order
        std::unordered_map<std::string, chen::cmd::action> _define;  // action defines

        std::unordered_map<std::string, std::string> _suggest;  // intelligent suggestion
    };
}