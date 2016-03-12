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
using namespace chen::cmd;

// -----------------------------------------------------------------------------
// parser
parser::parser(const std::string &app)
: _app(app)
{
}

// parse
void parser::parse(int argc, const char *const argv[])
{
    // argc must greater than 1
    if (argc <= 1)
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
            auto it = this->_define.find(cur ? cur->name() + "." + param : param);

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
            throw chen::cmd::error_action("cmd can not find current action");
    }

    std::unique_ptr<chen::cmd::action> action(new chen::cmd::action(*cur));

    // parse the option and object
    auto option = action->option();
    auto alias  = action->alias();

    std::string key;
    std::string val;

    std::vector<std::string> object;

    while (idx < argc)
    {
        auto param = argv[idx];

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
                else if (++idx < argc)
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

                key += *param++;

                if (*param == '=')
                {
                    while (*param)
                        val += *++param;
                }
                else if (++idx < argc)
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

                if (!val.empty())
                    opt->set(val);
                else
                    opt->set();
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
            ++idx;
        }
    }

    // the rest unresolved params
    this->_object = std::move(object);

    // store the current action
    this->_action = std::move(action);

    // call the callback if exist
    auto bind = this->_action->bind();
    if (bind)
        bind(*this);
}

// action
void parser::action(const std::string &name,
                    const std::string &desc,
                    std::function<void (const chen::cmd::parser &parser)> bind)
{
    this->_define.emplace(name, chen::cmd::action(name, desc, bind));
}

// option
void parser::option(const std::string &action,
                    const std::string &name,
                    const std::string &tiny,
                    const std::string &desc,
                    const any &def,
                    const any &pre)
{
    if (tiny.size() > 1)
        throw chen::cmd::error_option("cmd tiny name can be only one character");

    auto it = this->_define.find(action);

    if (it != this->_define.end())
        it->second.add(name, chen::cmd::option(name, tiny, desc, def, pre));
    else
        throw chen::cmd::error_action("cmd action not found");
}

// current
std::string parser::current() const
{
    if (this->_action)
        return this->_action->name();
    else
        throw chen::cmd::error_action("cmd current action not found");
}

// option value
bool parser::boolVal(const std::string &option) const
{
    if (this->_action)
    {
        auto temp = this->_action->option();
        auto find = temp.find(option);

        if (find != temp.end())
            return static_cast<bool>(find->second.val());
        else
            throw chen::cmd::error_option("cmd option not found: " + option);
    }
    else
    {
        throw chen::cmd::error_action("cmd current action not found");
    }
}

std::int32_t parser::intVal(const std::string &option) const
{
    if (this->_action)
    {
        auto temp = this->_action->option();
        auto find = temp.find(option);

        if (find != temp.end())
            return static_cast<std::int32_t>(find->second.val());
        else
            throw chen::cmd::error_option("cmd option not found: " + option);
    }
    else
    {
        throw chen::cmd::error_action("cmd current action not found");
    }
}

std::string parser::strVal(const std::string &option) const
{
    if (this->_action)
    {
        auto temp = this->_action->option();
        auto find = temp.find(option);

        if (find != temp.end())
            return find->second.val();
        else
            throw chen::cmd::error_option("cmd option not found: " + option);
    }
    else
    {
        throw chen::cmd::error_action("cmd current action not found");
    }
}

std::int64_t parser::int64Val(const std::string &option) const
{
    if (this->_action)
    {
        auto temp = this->_action->option();
        auto find = temp.find(option);

        if (find != temp.end())
            return static_cast<std::int64_t>(find->second.val());
        else
            throw chen::cmd::error_option("cmd option not found: " + option);
    }
    else
    {
        throw chen::cmd::error_action("cmd current action not found");
    }
}

double parser::doubleVal(const std::string &option) const
{
    if (this->_action)
    {
        auto temp = this->_action->option();
        auto find = temp.find(option);

        if (find != temp.end())
            return static_cast<double>(find->second.val());
        else
            throw chen::cmd::error_option("cmd option not found: " + option);
    }
    else
    {
        throw chen::cmd::error_action("cmd current action not found");
    }
}

// object value
const std::vector<std::string>& parser::objVal() const
{
    return this->_object;
}

// usage
void parser::usage() const
{
    if (!this->_prefix.empty())
        this->output(this->_prefix);

    // todo
    this->output("xxx");

    if (!this->_suffix.empty())
        this->output(this->_suffix);
}

void parser::prefix(const std::string &text)
{
    this->_prefix = text;
}

void parser::suffix(const std::string &text)
{
    this->_suffix = text;
}

void parser::suggest(const std::string &alias, const std::string &action)
{
    this->_suggest[alias] = action;
}

// output
void parser::output(const std::string &text) const
{
    std::cout << text << std::endl;
}


// -----------------------------------------------------------------------------
// action
action::action(const std::string &name,
               const std::string &desc,
               std::function<void (const chen::cmd::parser &parser)> bind)
: _name(name)
, _desc(desc)
, _bind(bind)
{
}

void action::add(const std::string &name, const chen::cmd::option &option)
{
    this->_option.emplace(name, option);
    this->_alias.emplace(option.tiny(), option.name());
}

const std::string& action::name() const
{
    return this->_name;
}

const std::string& action::desc() const
{
    return this->_desc;
}

const std::function<void (const chen::cmd::parser &parser)>& action::bind() const
{
    return this->_bind;
}

const std::map<std::string, chen::cmd::option>& action::option() const
{
    return this->_option;
}

const std::map<std::string, std::string>& action::alias() const
{
    return this->_alias;
};


// -----------------------------------------------------------------------------
// option
option::option(const std::string &name,
               const std::string &tiny,
               const std::string &desc,
               const chen::any &def,
               const chen::any &pre)
: _name(name)
, _tiny(tiny)
, _desc(desc)
, _def(def)
, _pre(pre)
{
}

void option::set()
{
    this->_set = true;
}

void option::set(const chen::any &val)
{
    this->_set = true;
    this->_val = val;
}

void option::clear()
{
    this->_set = false;
    this->_val = chen::any();
}

const std::string& option::name() const
{
    return this->_name;
}

const std::string& option::tiny() const
{
    return this->_tiny;
}

const std::string& option::desc() const
{
    return this->_desc;
}

const chen::any& option::val() const
{
    return this->_val;
}

const chen::any& option::def() const
{
    return this->_def;
}

const chen::any& option::pre() const
{
    return this->_pre;
}

const bool option::valid() const
{
    return this->_set;
}