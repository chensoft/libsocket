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
    int index = 1;
    std::string name;

    while (index < argc)
    {
        auto param = argv[index];

        if (param[0] != '-')
        {
            // sub-action is joined by dots
            std::string temp(!name.empty() ? name + "." + param : param);

            if (this->_define.find(temp) != this->_define.end())
                name = temp;
            else
                break;
        }
        else
        {
            break;
        }

        ++index;
    }

    // using empty action if not found
    auto it = this->_define.find(name);
    std::unique_ptr<chen::cmd::action> action(it != this->_define.end() ? new chen::cmd::action(it->second) : new chen::cmd::action);

    // parse the objects and options
    auto objects = action->objects();
    auto options = action->options();

    std::vector<chen::cmd::object>::iterator object = objects.begin();

    while (index < argc)
    {
        auto param = argv[index];

        if (param[0] == '-')
        {
            // handle option
            auto curr = (strlen(param) >= 2) && (param[1] == '-') ? std::string(param + 2) : std::string(param + 1);
            auto next = (index + 1 < argc) ? argv[index + 1] : nullptr;
            auto find = options.find(curr);

            if (find != options.end())
            {
                if (next && (next[0] != '-'))
                {
                    find->second.set(next);
                    index += 2;
                }
                else
                {
                    find->second.set();
                    ++index;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            // handle object
            if (object != objects.end())
            {
                auto len = object->size();
                auto cur = index;

                while ((index < argc) && (index - cur < len))
                {
                    auto temp = argv[index];

                    if (temp[0] != '-')
                        object->add(temp);
                    else
                        break;

                    ++index;
                }

                // restore if the object is not reach len
                auto size = object->val().size();

                if (len && (size < len))
                {
                    // length is error
                    index = cur;
                    object->clear();
                    break;
                }
                else
                {
                    ++object;
                }
            }
            else
            {
                break;
            }
        }
    }

    // the rest unresolved params
    this->_rest.clear();

    for (; index < argc; ++index)
        this->_rest.push_back(argv[index]);

    // store the current action
    this->_action.swap(action);
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
                    std::size_t size)
{
    auto it = this->_define.find(action);

    if (it != this->_define.end())
        it->second.add(chen::cmd::object(name, size));
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
        return find != temp.end() ? static_cast<bool>(find->second.val()) : false;
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
        return find != temp.end() ? static_cast<std::int32_t>(find->second.val()) : 0;
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
        return find != temp.end() ? static_cast<std::int64_t>(find->second.val()) : 0;
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
        return find != temp.end() ? static_cast<double>(find->second.val()) : 0.0;
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
object::object(const std::string &name, std::size_t size)
: _name(name)
, _size(size)
{
}

void object::add(const std::string &val)
{
    this->_val.push_back(val);
}

void object::clear()
{
    this->_val.clear();
}

const std::string& object::name() const
{
    return this->_name;
}

std::size_t object::size() const
{
    return this->_size;
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