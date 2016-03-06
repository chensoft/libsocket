/**
 * Created by Jian Chen
 * @since  2016.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "cmd.hpp"
#include "log.hpp"
#include "path.hpp"
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
}

// action
void parser::action(const std::string &name,
                    const std::string &desc,
                    std::function<void (const chen::cmd::parser &parser)> bind)
{
    this->_define.emplace(name, chen::cmd::action(name, desc, bind));
}

// object
void parser::object(const std::string &action,
                    const std::string &name,
                    std::size_t min,
                    std::size_t max)
{
    auto it = this->_define.find(action);

    if (it != this->_define.end())
        it->second.add(chen::cmd::object(name, min, max));
    else
        throw chen::cmd::error_action("cmd action not found");
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
        throw chen::cmd::error("cmd tiny name can be only one character");

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

// object value
std::vector<std::string> parser::objVal(const std::string &object) const
{
    if (this->_action)
    {
        auto temp = this->_action->objects();

        for (auto &obj : temp)
        {
            if (obj.name() == object)
            {
                return obj.val();
            }
        }

        return std::vector<std::string>();
    }
    else
    {
        throw chen::cmd::error_action("cmd current action not found");
    }
}

// option value
bool parser::boolVal(const std::string &option) const
{
    if (this->_action)
    {
        auto temp = this->_action->options();
        auto find = temp.find(option);
        return find != temp.end() ? find->second.val() : false;
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
        auto temp = this->_action->options();
        auto find = temp.find(option);
        return find != temp.end() ? find->second.val() : 0;
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
        auto temp = this->_action->options();
        auto find = temp.find(option);
        return find != temp.end() ? find->second.val() : "";
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
        auto temp = this->_action->options();
        auto find = temp.find(option);
        return find != temp.end() ? find->second.val() : 0;
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
        auto temp = this->_action->options();
        auto find = temp.find(option);
        return find != temp.end() ? find->second.val() : 0.0;
    }
    else
    {
        throw chen::cmd::error_action("cmd current action not found");
    }
}

// rest
const std::vector<std::string>& parser::rest() const
{
    return this->_rest;
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

void action::add(const chen::cmd::object &object)
{
    this->_objects.push_back(object);
}

void action::add(const std::string &name, const chen::cmd::option &option)
{
    this->_options.emplace(name, option);
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

const std::vector<chen::cmd::object>& action::objects() const
{
    return this->_objects;
}

const std::map<std::string, chen::cmd::option>& action::options() const
{
    return this->_options;
}


// -----------------------------------------------------------------------------
// object
object::object(const std::string &name, std::size_t min, std::size_t max)
: _name(name)
, _min(min)
, _max(max)
{
}

const std::string& object::name() const
{
    return this->_name;
}

std::size_t object::min() const
{
    return this->_min;
}

std::size_t object::max() const
{
    return this->_max;
}

const std::vector<std::string>& object::val() const
{
    return this->_val;
}


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