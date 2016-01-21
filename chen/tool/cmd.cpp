/**
 * Created by Jian Chen
 * @since  2016.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "cmd.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// cmd
cmd::cmd(const std::string &app)
: _app(app)
{

}

// parse
void cmd::parse(int argc, const char *const argv[])
{
    // todo throw
    if (argc <= 1)
        return;

    // app name, if empty then use the first argument
    if (this->_app.empty())
        this->_app = argv[0];
}

// action
void cmd::action(const std::string &name,
                 const std::string &desc,
                 std::function<void (const chen::cmd &cmd)> bind)
{

}

// object
void cmd::object(const std::string &action, int limit)
{

}

// option
void cmd::option(const std::string &action,
                 const std::string &full,
                 const std::string &tiny,
                 const std::string &desc,
                 const any &def,
                 const any &pre)
{

}

// matched
std::string cmd::action() const
{
    return "";
}

// rest
const std::vector<std::string>& cmd::rest() const
{
    return this->_rest;
}

// value
bool cmd::boolVal(const std::string &option) const
{
    return false;
}

int cmd::intVal(const std::string &option) const
{
    return 0;
}

std::string cmd::strVal(const std::string &option) const
{
    return "";
}

long long cmd::int64Val(const std::string &option) const
{
    return 0;
}

double cmd::doubleVal(const std::string &option) const
{
    return 0.0;
}

// usage
void cmd::usage() const
{
    this->output("xxx");
}

void cmd::prefix(const std::string &text)
{
    this->_prefix = text;
}

void cmd::suffix(const std::string &text)
{
    this->_suffix = text;
}

void cmd::suggest(const std::string &alias, const std::string &action)
{

}

// output
void cmd::output(const std::string &text) const
{
    std::cout << text << std::endl;
}


// -----------------------------------------------------------------------------
// action
cmd::action::action(const std::string &name,
                    const std::string &desc,
                    std::function<void (const chen::cmd &cmd)> bind)
: _name(name)
, _desc(desc)
, _bind(bind)
{

}


// -----------------------------------------------------------------------------
// object
cmd::object::object(int limit)
: _limit(limit)
{

}


// -----------------------------------------------------------------------------
// option
cmd::option::option(const std::string &full,
                    const std::string &tiny,
                    const std::string &desc,
                    const chen::any &def,
                    const chen::any &pre)
: _full(full)
, _tiny(tiny)
, _desc(desc)
, _def(def)
, _pre(pre)
{

}