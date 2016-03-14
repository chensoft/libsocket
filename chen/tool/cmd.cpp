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
        throw chen::cmd::error_general("cmd action already exist");
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
        throw chen::cmd::error_general("cmd full name can't be null");

    // short name must be 1 character
    if (tiny.size() > 1)
        throw chen::cmd::error_general("cmd tiny name must be a character");

    // don't allow duplicate option
    auto &option = this->_cursor->option;
    auto &alias  = this->_cursor->alias;

    if (option.find(name) != option.end())
        throw chen::cmd::error_general("cmd option full name already exist");
    else if (!tiny.empty() && (alias.find(tiny) != alias.end()))
        throw chen::cmd::error_general("cmd option tiny name already exist");

    // add this option
    chen::cmd::option opt;
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
        throw chen::cmd::error_general("cmd action already exist");
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
            throw chen::cmd::error_general("cmd can not find current action");
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

                if (*param && (*(param + 1) == '\0'))
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
                chen::cmd::option *opt = nullptr;

                if (key.size() > 1)
                {
                    // long
                    auto it = option.find(key);
                    if (it != option.end())
                        opt = &it->second;
                    else
                        throw chen::cmd::error_parse("cmd option not found: " + key, key);
                }
                else
                {
                    // short
                    auto it = alias.find(key);
                    if (it != alias.end())
                        opt = &option[it->second];
                    else
                        throw chen::cmd::error_parse("cmd option not found: " + key, key);
                }
                
                opt->set = true;

                if (!val.empty())
                    opt->val = val;
                else
                    opt->val = "true";  // treat null value option as boolean
            }
            else
            {
                throw chen::cmd::error_parse("cmd option is empty", "");
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

// app
std::string cmd::app() const
{
    return this->_app;
}

// current
std::string cmd::current() const
{
    if (this->_action)
        return this->_action->name;
    else
        throw chen::cmd::error_general("cmd current action not found");
}

// option value
bool cmd::boolVal(const std::string &name) const
{
    auto opt = this->opt(name);
    return opt.set ? (opt.val == "true") : static_cast<bool>(opt.def);
}

std::int32_t cmd::intVal(const std::string &name) const
{
    auto opt = this->opt(name);
    return opt.set ? std::atoi(opt.val.c_str()) : static_cast<std::int32_t>(opt.def);
}

std::string cmd::strVal(const std::string &name) const
{
    auto opt = this->opt(name);

    if (opt.set)
    {
        return opt.val;
    }
    else
    {
        auto tmp1 = static_cast<const char*>(opt.def);
        if (tmp1)
            return tmp1;

        auto tmp2 = static_cast<char*>(opt.def);
        if (tmp2)
            return tmp2;

        std::string tmp3 = opt.def;
        return tmp3;
    }
}

std::int64_t cmd::int64Val(const std::string &name) const
{
    auto opt = this->opt(name);
    return opt.set ? std::atoll(opt.val.c_str()) : static_cast<std::int64_t>(opt.def);
}

double cmd::doubleVal(const std::string &name) const
{
    auto opt = this->opt(name);
    return opt.set ? std::atof(opt.val.c_str()) : static_cast<double>(opt.def);
}

// object value
const std::vector<std::string>& cmd::object() const
{
    return this->_object;
}

// usage
std::string cmd::usage() const
{
    // todo
    return "xxx";
}

std::string cmd::usage(const chen::cmd::error_parse &error) const
{
    // todo
    return "";
}

void cmd::visit(std::function<void (const chen::cmd::action &action)> callback,
                std::function<bool (const std::string &a, const std::string &b)> compare) const
{
    // sort the keys
    std::vector<std::string> keys;
    for (auto it : this->_define)
        keys.push_back(it.first);

    if (compare)
        std::sort(keys.begin(), keys.end(), compare);
    else
        std::sort(keys.begin(), keys.end());

    // visit actions
    for (auto &str : keys)
        callback(this->_define.at(str));
}

void cmd::visit(const std::string &action,
                std::function<void (const chen::cmd::option &option)> callback,
                std::function<bool (const std::string &a, const std::string &b)> compare) const
{
    auto find = this->_define.find(action);
    if (find == this->_define.end())
        throw chen::cmd::error_general("cmd action not found: " + action);

    // sort the keys
    auto &act = find->second;
    std::vector<std::string> keys;

    for (auto it : act.option)
        keys.push_back(it.first);

    if (compare)
        std::sort(keys.begin(), keys.end(), compare);
    else
        std::sort(keys.begin(), keys.end());

    // visit options
    for (auto &str : keys)
        callback(act.option.at(str));
}

void cmd::suggest(const std::string &alias, const std::string &action)
{
    this->_suggest[alias] = action;
}

// option
const chen::cmd::option& cmd::opt(const std::string &name) const
{
    if (this->_action)
    {
        auto &option = this->_action->option;
        auto &alias  = this->_action->alias;

        auto it = option.find(name);

        if ((it == option.end()) && (name.size() == 1))
        {
            auto temp = alias.find(name);
            if (temp != alias.end())
                it = option.find(temp->second);
        }

        if (it != option.end())
            return it->second;
        else
            throw chen::cmd::error_general("cmd option not found: " + name);
    }
    else
    {
        throw chen::cmd::error_general("cmd current action not found");
    }
}