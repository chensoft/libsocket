/**
 * Created by Jian Chen
 * @since  2016.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "cmd.hpp"
#include "path.hpp"

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
    // first param must be app name
    if (argc < 1)
        throw chen::cmd::error("cmd count is zero");

    // if app is empty then use first argument
    if (this->_app.empty())
        this->_app = argv[0];

    // if param count is 1 then return
    if (argc == 1)
        return;

    // check action
    int index = 1;
    std::string action;

    while (index < argc)
    {
        auto param = argv[index];

        if (param && (param[0] == '-'))
            break;
        else
            action = !action.empty() ? action + "." + param : action + param;

        ++index;
    }
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
                    int min,
                    int max)
{
    auto it = this->_define.find(action);

    if (it != this->_define.end())
        it->second.add(chen::cmd::object(name, min, max));
    else
        throw chen::cmd::error("cmd define object action not found");
}

// option
void parser::option(const std::string &action,
                    const std::string &name,
                    const std::string &tiny,
                    const std::string &desc,
                    const any &def,
                    const any &pre)
{
    auto it = this->_define.find(action);

    if (it != this->_define.end())
        it->second.add(name, chen::cmd::option(name, tiny, desc, def, pre));
    else
        throw chen::cmd::error("cmd define option action not found");
}

// current
std::string parser::current() const
{
    return this->_action;
}

// object value
std::vector<std::string> parser::objVal(const std::string &object) const
{
    auto it = this->_define.find(this->_action);

    if (it != this->_define.end())
    {
        auto temp = it->second.objects();

        for (auto &obj : temp)
        {
            if (obj.name() == object)
            {
                return obj.val();
            }
        }

        throw chen::cmd::error("cmd object value object not found");
    }
    else
    {
        throw chen::cmd::error("cmd object value current action not found");
    }
}

// option value
bool parser::boolVal(const std::string &option) const
{
    auto it = this->_define.find(this->_action);

    if (it != this->_define.end())
    {
        auto temp = it->second.options();
        auto find = temp.find(option);

        if (find != temp.end())
            return find->second.val();

        throw chen::cmd::error("cmd bool value option not found");
    }
    else
    {
        throw chen::cmd::error("cmd bool value current action not found");
    }
}

std::int32_t parser::intVal(const std::string &option) const
{
    auto it = this->_define.find(this->_action);

    if (it != this->_define.end())
    {
        auto temp = it->second.options();
        auto find = temp.find(option);

        if (find != temp.end())
            return find->second.val();

        throw chen::cmd::error("cmd int value option not found");
    }
    else
    {
        throw chen::cmd::error("cmd int value current action not found");
    }
}

std::string parser::strVal(const std::string &option) const
{
    auto it = this->_define.find(this->_action);

    if (it != this->_define.end())
    {
        auto temp = it->second.options();
        auto find = temp.find(option);

        if (find != temp.end())
            return find->second.val();

        throw chen::cmd::error("cmd str value option not found");
    }
    else
    {
        throw chen::cmd::error("cmd str value current action not found");
    }
}

std::int64_t parser::int64Val(const std::string &option) const
{
    auto it = this->_define.find(this->_action);

    if (it != this->_define.end())
    {
        auto temp = it->second.options();
        auto find = temp.find(option);

        if (find != temp.end())
            return find->second.val();

        throw chen::cmd::error("cmd int64 value option not found");
    }
    else
    {
        throw chen::cmd::error("cmd int64 value current action not found");
    }
}

double parser::doubleVal(const std::string &option) const
{
    auto it = this->_define.find(this->_action);

    if (it != this->_define.end())
    {
        auto temp = it->second.options();
        auto find = temp.find(option);

        if (find != temp.end())
            return find->second.val();

        throw chen::cmd::error("cmd double value option not found");
    }
    else
    {
        throw chen::cmd::error("cmd double value current action not found");
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
object::object(const std::string &name, int min, int max)
: _name(name)
, _min(min)
, _max(max)
{
}

const std::string& object::name() const
{
    return this->_name;
}

int object::min() const
{
    return this->_min;
}

int object::max() const
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