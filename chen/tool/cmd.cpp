/**
 * Created by Jian Chen
 * @since  2016.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "cmd.hpp"
#include "log.hpp"
#include "path.hpp"
#include <string.h>
#include <iostream>

using namespace chen;

// -----------------------------------------------------------------------------
// cmd
cmd::cmd(const std::string &app)
: _app(app)
{
    // create root action
    this->create("", "");
}

// action
void cmd::create(const std::string &name,
                 const std::string &desc,
                 std::function<void (const chen::cmd &cmd)> bind)
{
    if (this->_define.find(name) == this->_define.end())
    {
        chen::cmd::action action;
        action.name = name;
        action.desc = desc;
        action.bind = bind;
        this->_cursor = &this->_define.emplace(name, action).first->second;
    }
    else
    {
        throw std::runtime_error("cmd action already exist");
    }
}

// define
void cmd::define(const std::string &name,
                 const std::string &tiny,
                 const std::string &desc,
                 const any &def)
{
    // full name can't be null
    if (name.empty())
        throw std::runtime_error("cmd full name can't be null");

    // short name must be 1 character
    if (tiny.size() > 1)
        throw std::runtime_error("cmd tiny name must be a character");

    // don't allow duplicate option
    auto &option = this->_cursor->option;
    auto &alias  = this->_cursor->alias;

    if (option.find(name) != option.end())
        throw std::runtime_error("cmd option full name already exist");
    else if (!tiny.empty() && (alias.find(tiny) != alias.end()))
        throw std::runtime_error("cmd option tiny name already exist");

    // add this option
    struct chen::cmd::option opt;
    opt.name = name;
    opt.tiny = tiny;
    opt.desc = desc;
    opt.def  = def;

    option[name] = opt;

    if (!tiny.empty())
        alias[tiny] = name;
}

void cmd::change(const std::string &name)
{
    auto it = this->_define.find(name);

    if (it != this->_define.end())
        this->_cursor = &it->second;
    else
        throw std::runtime_error("cmd action already exist");
}

// parse
void cmd::parse(int argc, const char *const argv[])
{
    // argc must greater than 0
    if (argc < 1)
        return;

    // set app name if it's empty
    if (this->_app.empty())
        this->_app = path::basename(argv[0]);

    // find current action first
    int idx = 1;
    chen::cmd::action *cur = nullptr;

    while (idx < argc)
    {
        auto param = argv[idx];

        if (param[0] != '-')
        {
            // sub-action is joined by dots
            auto it = this->_define.find(cur ? cur->name + "." + param : param);

            if (it != this->_define.end())
                cur = &it->second;
            else
                break;
        }
        else
        {
            break;
        }

        ++idx;
    }

    // find the root action
    if (!cur)
    {
        auto it = this->_define.find("");

        if (it != this->_define.end())
            cur = &it->second;
        else
            throw std::runtime_error("cmd can not find current action");
    }

    std::unique_ptr<chen::cmd::action> action(new chen::cmd::action(*cur));

    // parse the option and object
    auto &option = action->option;
    auto &alias  = action->alias;

    std::string key;
    std::string val;

    std::vector<std::string> object;

    while (idx < argc)
    {
        auto param = argv[idx++];

        if (param[0] == '-')
        {
            // parse option
            if (param[1] == '-')
            {
                // long option
                // e.g: --help, --user=root, --user root
                param += 2;

                while (*param && (*param != '='))
                    key += *param++;

                if (*param == '=')
                {
                    while (*param)
                        val += *++param;
                }
                else if (idx < argc)
                {
                    param = argv[idx];

                    if (param[0] != '-')
                    {
                        val = param;
                        ++idx;
                    }
                }
            }
            else
            {
                // short option
                // e.g: -h, -u=root, -u root
                ++param;

                if (*param)
                    key += *param++;

                if (*param == '=')
                {
                    while (*param)
                        val += *++param;
                }
                else if (idx < argc)
                {
                    param = argv[idx];

                    if (param[0] != '-')
                    {
                        val = param;
                        ++idx;
                    }
                }
            }

            // handle option
            if (!key.empty())
            {
                struct chen::cmd::option *opt = nullptr;

                if (key.size() > 1)
                {
                    // long
                    auto it = option.find(key);
                    if (it != option.end())
                        opt = &it->second;
                    else
                        ;  // todo show option not found usage
                }
                else
                {
                    // short
                    auto it = alias.find(key);
                    if (it != alias.end())
                        opt = &option[it->second];
                    else
                        ;  // todo show option not found usage
                }
                
                opt->set = true;

                if (!val.empty())
                    opt->val = val;
                else
                    opt->val = "true";  // treat null value option as boolean
            }
            else
            {
                // todo show error option key not found
            }

            // clear key & val
            key.clear();
            val.clear();
        }
        else
        {
            // handle object
            object.push_back(param);
        }
    }

    // the rest unresolved params
    this->_object = std::move(object);

    // store the current action
    this->_action = std::move(action);

    // call the callback if exist
    auto &bind = this->_action->bind;
    if (bind)
        bind(*this);
}

// current
std::string cmd::current() const
{
    if (this->_action)
        return this->_action->name;
    else
        throw std::runtime_error("cmd current action not found");
}

// option value
bool cmd::boolVal(const std::string &name) const
{
    auto option = this->option(name);
    return option.set ? (option.val == "true") : static_cast<bool>(option.def);
}

std::int32_t cmd::intVal(const std::string &name) const
{
    auto option = this->option(name);
    return option.set ? std::atoi(option.val.c_str()) : static_cast<std::int32_t>(option.def);
}

std::string cmd::strVal(const std::string &name) const
{
    auto option = this->option(name);

    if (option.set)
    {
        return option.val;
    }
    else
    {
        auto tmp1 = static_cast<const char*>(option.def);
        if (tmp1)
            return tmp1;

        auto tmp2 = static_cast<char*>(option.def);
        if (tmp2)
            return tmp2;

        std::string tmp3 = option.def;
        return tmp3;
    }
}

std::int64_t cmd::int64Val(const std::string &name) const
{
    auto option = this->option(name);
    return option.set ? std::atoll(option.val.c_str()) : static_cast<std::int64_t>(option.def);
}

double cmd::doubleVal(const std::string &name) const
{
    auto option = this->option(name);
    return option.set ? std::atof(option.val.c_str()) : static_cast<double>(option.def);
}

// object value
const std::vector<std::string>& cmd::object() const
{
    return this->_object;
}

// app
std::string cmd::app() const
{
    return this->_app;
}

// usage
std::string cmd::usage() const
{
    return "xxx";
}

void cmd::suggest(const std::string &alias, const std::string &action)
{
    this->_suggest[alias] = action;
}

// option
const struct chen::cmd::option& cmd::option(const std::string &name) const
{
    if (this->_action)
    {
        auto &option = this->_action->option;
        auto &alias  = this->_action->alias;

        auto it = option.find(name);
        if ((it == option.end()) && (name.size() == 1))
            it = option.find(alias[name]);

        if (it != option.end())
            return it->second;
        else
            throw std::runtime_error("cmd option not found: " + name);
    }
    else
    {
        throw std::runtime_error("cmd current action not found");
    }
}