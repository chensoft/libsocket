/**
 * Created by Jian Chen
 * @since  2016.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/tool/cmd.hpp>
#include <chen/base/num.hpp>
#include <chen/base/map.hpp>
#include <chen/sys/fs.hpp>
#include <algorithm>
#include <cstdlib>

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
void cmd::create(const std::string &action,
                 const std::string &desc,
                 std::function<void (const chen::cmd &cmd)> bind)
{
    auto it = this->_define.find(action);

    if (it == this->_define.end())
    {
        chen::cmd::action act;
        act.name = action;
        act.desc = desc;
        act.bind = bind;
        this->_cursor = &this->_define.emplace(action, std::move(act)).first->second;
    }
    else
    {
        this->_cursor = &it->second;
        this->_cursor->name = action;
        this->_cursor->desc = desc;
        this->_cursor->bind = bind;
    }
}

// define
void cmd::define(const std::string &option,
                 const std::string &tiny,
                 const std::string &desc,
                 const any &def)
{
    // full name can't be null
    if (option.empty())
        throw chen::cmd::error_general("cmd: full name can't be null");

    // full name must greater than 1
    if (option.size() == 1)
        throw chen::cmd::error_general("cmd: full name size must greater than 1");

    // short name must be empty or 1 character
    if (tiny.size() > 1)
        throw chen::cmd::error_general("cmd: tiny name must be a character");

    // don't allow duplicate option
    auto &options = this->_cursor->options;
    auto &alias   = this->_cursor->alias;

    if (options.find(option) != options.end())
        throw chen::cmd::error_general("cmd: option full name already exist");
    else if (!tiny.empty() && (alias.find(tiny) != alias.end()))
        throw chen::cmd::error_general("cmd: option tiny name already exist");

    // add this option
    chen::cmd::option opt;
    opt.name = option;
    opt.tiny = tiny;
    opt.desc = desc;
    opt.def  = def;

    options[option] = opt;

    if (!tiny.empty())
        alias[tiny] = option;
}

// change
void cmd::change(const std::string &action)
{
    auto it = this->_define.find(action);

    if (it != this->_define.end())
        this->_cursor = &it->second;
    else
        throw chen::cmd::error_general("cmd: action not found: " + action);
}

// exist
bool cmd::exist(const std::string &action, const std::string &option) const
{
    auto it = this->_define.find(action);
    if (it == this->_define.end())
        return false;

    return option.empty() ? true : (it->second.options.find(option) != it->second.options.end());
}

// parse
void cmd::parse(int argc, const char *const argv[])
{
    // argc must greater than 0
    if (argc < 1)
        return;

    // set app name if it's empty
    if (this->_app.empty())
        this->_app = fs::basename(argv[0]);

    // find the current action
    int idx = 1;
    chen::cmd::action *cur = nullptr;

    if ((argc == 1) || (argv[1][0] == '-') || (this->_define.size() == 1))
    {
        // use root action
        cur = &this->_define[""];
    }
    else
    {
        // find action, throw error if not found
        auto param = argv[idx++];
        auto find  = this->_define.find(param);

        if (find != this->_define.end())
        {
            // find sub-actions
            cur = &find->second;

            while (idx < argc)
            {
                param = argv[idx];

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
        }
        else
        {
            throw chen::cmd::error_parse("cmd: action not found", param, "");
        }
    }

    std::unique_ptr<chen::cmd::action> action(new chen::cmd::action(*cur));

    // parse the options and objects
    auto &options = action->options;
    auto &alias   = action->alias;

    std::string key;
    std::string val;

    std::vector<std::string> objects;

    while (idx < argc)
    {
        auto param = argv[idx++];
        auto store = param;

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

                // long option name must greater than 1
                if (key.size() == 1)
                    key.clear();

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

                if (*param && ((*(param + 1) == '\0') || (*(param + 1) == '=')))
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
                    auto it = options.find(key);
                    if (it != options.end())
                        opt = &it->second;
                    else
                        throw chen::cmd::error_parse("cmd: option not found: " + key, action->name, store);
                }
                else
                {
                    // short
                    auto it = alias.find(key);
                    if (it != alias.end())
                        opt = &options[it->second];
                    else
                        throw chen::cmd::error_parse("cmd: option not found: " + key, action->name, store);
                }

                opt->set = true;

                if (!val.empty())
                    opt->val = val;
                else
                    opt->val = "true";  // treat null value option as boolean
            }
            else
            {
                throw chen::cmd::error_parse("cmd: option is empty", action->name, store);
            }

            // clear key & val
            key.clear();
            val.clear();
        }
        else
        {
            // handle object
            objects.push_back(param);
        }
    }

    // the rest unresolved params
    this->_objects = std::move(objects);

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
        throw chen::cmd::error_general("cmd: current action not found");
}

// option value
bool cmd::boolVal(const std::string &option) const
{
    auto opt = this->opt(option);
    return opt.set ? (opt.val == "true") : static_cast<bool>(opt.def);
}

std::int32_t cmd::intVal(const std::string &option) const
{
    auto opt = this->opt(option);
    return opt.set ? std::atoi(opt.val.c_str()) : static_cast<std::int32_t>(opt.def);
}

std::string cmd::strVal(const std::string &option) const
{
    auto opt = this->opt(option);

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

        return opt.def;
    }
}

std::int64_t cmd::int64Val(const std::string &option) const
{
    auto opt = this->opt(option);
    return opt.set ? std::atoll(opt.val.c_str()) : static_cast<std::int64_t>(opt.def);
}

double cmd::doubleVal(const std::string &option) const
{
    auto opt = this->opt(option);
    return opt.set ? std::atof(opt.val.c_str()) : static_cast<double>(opt.def);
}

// object value
const std::vector<std::string>& cmd::objects() const
{
    return this->_objects;
}

// usage
std::string cmd::usage() const
{
    if (this->_define.size() == 1)
    {
        // has root action, show root's options
        return this->usage("");
    }
    else
    {
        // multiple actions, show all actions
        std::string ret("Actions:\n");
        std::size_t max = 0;  // max width

        this->visit([&max] (const chen::cmd::action &action, std::size_t idx, std::size_t len) {
            max = std::max(max, action.name.size());
        });

        std::string fmt("  %-" + chen::num::str(max + 2) + "s%s");  // action + spacing

        this->visit([&ret, &fmt] (const chen::cmd::action &action, std::size_t idx, std::size_t len) {
            if (!action.name.empty())
            {
                ret += str::format(fmt.c_str(), action.name.c_str(), action.desc.c_str());

                if (idx < len - 1)
                    ret += "\n";
            }
        });

        return ret;
    }
}

std::string cmd::usage(const std::string &action) const
{
    if (this->_define.find(action) == this->_define.end())
        return this->usage(action, "");

    std::string ret("Options:\n");
    std::size_t full = 0;  // full name width

    this->visit(action, [&full] (const chen::cmd::option &option, std::size_t idx, std::size_t len) {
        full = std::max(full, option.name.size());
    });

    std::string fmt("--%-" + chen::num::str(full + 2) + "s%s");

    this->visit(action, [&ret, &fmt] (const chen::cmd::option &option, std::size_t idx, std::size_t len) {
        // tiny name
        if (!option.tiny.empty())
            ret += "  -" + option.tiny + " ";
        else
            ret += "     ";

        // full name
        ret += str::format(fmt.c_str(), option.name.c_str(), option.desc.c_str());

        if (idx < len - 1)
            ret += "\n";
    });

    return ret;
}

std::string cmd::usage(const std::string &action, const std::string &option) const
{
    if (!option.empty())
    {
        // option not found
        return str::format("Error: unknown option \"%s\"", option.c_str());
    }
    else
    {
        // action not found
        std::string ret(str::format("Error: unknown action \"%s\" for \"%s\"", action.c_str(), this->_app.c_str()));

        // give more hint
        if (this->_define.size() > 1)
        {
            std::vector<std::string> hint;

            do
            {
                // direct get suggest actions
                auto it = this->_suggest.find(action);

                if (it != this->_suggest.end())
                {
                    hint.push_back(it->second);
                    break;
                }

                // try levenshtein algorithm
                std::size_t size = action.size();
                std::size_t cost = std::numeric_limits<std::size_t>::max();

                for (auto p : this->_define)
                {
                    if (p.first.empty())
                        continue;

                    std::size_t temp = str::levenshtein(action.c_str(),
                                                        action.size(),
                                                        p.first.c_str(),
                                                        p.first.size());

                    if (temp < std::max(size, p.first.size()))
                    {
                        if (temp < cost)
                        {
                            cost = temp;

                            hint.clear();
                            hint.push_back(p.first);
                        }
                        else if (temp == cost)
                        {
                            hint.push_back(p.first);
                        }
                    }
                }

                // try suggest actions
                for (auto p : this->_suggest)
                {
                    std::size_t temp = str::levenshtein(action.c_str(),
                                                        action.size(),
                                                        p.first.c_str(),
                                                        p.first.size());

                    if (temp < std::max(size, p.first.size()))
                    {
                        if (temp < cost)
                        {
                            cost = temp;

                            hint.clear();
                            hint.push_back(p.second);
                        }
                        else if (temp == cost)
                        {
                            hint.push_back(p.second);
                        }
                    }
                }
            } while (false);

            if (!hint.empty())
            {
                ret += "\n\nDid you mean this?\n";

                for (std::size_t i = 0, len = hint.size(); i < len; ++i)
                    ret += "        " + hint[i] + (i < len -1 ? "\n" : "");
            }
        }

        return ret;
    }
}

std::string cmd::usage(const chen::cmd::error_parse &error) const
{
    return this->usage(error.action, error.option);
}

void cmd::visit(std::function<void (const chen::cmd::action &action, std::size_t idx, std::size_t len)> callback,
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
    for (std::size_t i = 0, len = keys.size(); i < len; ++i)
        callback(this->_define.at(keys[i]), i, len);
}

void cmd::visit(const std::string &action,
                std::function<void (const chen::cmd::option &option, std::size_t idx, std::size_t len)> callback,
                std::function<bool (const std::string &a, const std::string &b)> compare) const
{
    auto find = this->_define.find(action);
    if (find == this->_define.end())
        throw chen::cmd::error_general("cmd: action not found: " + action);

    // sort the keys
    auto &act = find->second;
    auto keys = chen::map::keys(act.options);

    if (compare)
        std::sort(keys.begin(), keys.end(), compare);
    else
        std::sort(keys.begin(), keys.end());

    // visit options
    for (std::size_t i = 0, len = keys.size(); i < len; ++i)
        callback(act.options.at(keys[i]), i, len);
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
        auto &options = this->_action->options;
        auto &alias   = this->_action->alias;

        auto it = options.find(name);

        if ((it == options.end()) && (name.size() == 1))
        {
            auto temp = alias.find(name);
            if (temp != alias.end())
                it = options.find(temp->second);
        }

        if (it != options.end())
            return it->second;
        else
            throw chen::cmd::error_general("cmd: option not found: " + name);
    }
    else
    {
        throw chen::cmd::error_general("cmd: current action not found");
    }
}