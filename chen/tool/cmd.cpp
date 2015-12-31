/**
 * Created by Jian Chen
 * @since  2015.08.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "cmd.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// option base
std::string cmd_option_base::name() const
{
    return this->_name;
}

std::string cmd_option_base::tiny() const
{
    return this->_tiny;
}

std::string cmd_option_base::desc() const
{
    return this->_desc;
}

void cmd_option_base::setParse(bool parse)
{
    this->_parse = parse;
}


// -----------------------------------------------------------------------------
// cmd
void cmd::parse(int argc, const char *const argv[])
{
    // clear
    this->clear();

    int idx = 1;  // skip the program itself

    while (idx < argc)
    {
        std::string text(argv[idx]);
        std::string name;
        std::string value;

        bool mixed = false;

        if (text[0] != '-')
        {
            // :-) program command
            name  = text;
            value = "";
        }
        else
        {
            auto find = text.find_first_of("=");

            if ((text.size() > 1) && (text[1] == '-'))
            {
                // long option
                if (find != std::string::npos)
                {
                    // :-) program --option=<argument>
                    name  = text.substr(2, find - 2);
                    value = text.substr(find + 1);
                }
                else
                {
                    // :-) program --option <argument>
                    name = text.substr(2);

                    if (((idx + 1) < argc) && (argv[idx + 1][0] != '-'))
                    {
                        value = argv[++idx];  // forward
                    }
                }
            }
            else
            {
                // short option
                // if option has argument then the name if full
                // if no argument, the name may be mixed, e.g: "ls -al" is equal to "ls -a -l"
                if (find != std::string::npos)
                {
                    // :-) program -option=<argument>
                    name  = text.substr(1, find - 1);
                    value = text.substr(find + 1);
                }
                else
                {
                    if (((idx + 1) < argc) && (argv[idx + 1][0] != '-'))
                    {
                        // :-) program -option <argument>
                        name  = text.substr(1);
                        value = argv[++idx];  // forward
                    }
                    else
                    {
                        // :-) program -abc
                        name  = text.substr(1);
                        mixed = true;
                    }
                }
            }
        }

        // handle name & value
        if (mixed)
        {
            // try full name first
            try
            {
                this->update(text, name, "");
            }
            catch (...)
            {
                // split to single chars
                for (std::string::size_type i = 0, len = name.size(); i < len; ++i)
                    this->update(text, std::string(1, name[i]), "");
            }
        }
        else
        {
            this->update(text, name, value);
        }

        // increase idx
        ++idx;
    }
}

std::string cmd::usage()
{
    std::stringstream ss;

    ss << "Options:" << std::endl;

    // find the maximum indent
    std::string::size_type indent = 0;

    for (auto &it : this->_store)
    {
        auto name  = it.second->name();
        auto tiny = it.second->tiny();
        auto desc  = it.second->desc();

        if (tiny.empty())
            indent = std::max(indent, name.size() + 8);  // e.g: "      --addr"
        else
            indent = std::max(indent, tiny.size() + name.size() + 7);  // e.g: "  -a, --addr"
    }

    // plus two space indent
    indent += 2;

    // print the options
    for (auto it = this->_store.begin();;)
    {
        auto name = it->second->name();
        auto tiny = it->second->tiny();
        auto desc = it->second->desc();

        if (tiny.empty())
            ss << "      --" << name << std::string(indent - name.size() - 8, ' ');
        else
            ss << "  -" << tiny << ", --" << name << std::string(indent - tiny.size() - name.size() - 7, ' ');

        if (++it != this->_store.end())
        {
            ss << desc << std::endl;
        }
        else
        {
            // last
            ss << desc;
            break;
        }
    }

    return ss.str();
}

void cmd::clear()
{
    for (auto &it : this->_store)
        it.second->setParse(false);
}

void cmd::update(const std::string &text, const std::string &name, const std::string &value)
{
    std::string tiny;

    auto it_tiny = this->_tiny.find(name);

    if (it_tiny != this->_tiny.end())
        tiny = it_tiny->second;
    else
        tiny = name;

    auto it_store = this->_store.find(tiny);

    if (it_store == this->_store.end())
        throw std::runtime_error("cmd: parse option but not defined: " + text + ", name: " + name);

    try
    {
        it_store->second->setValue(value);
        it_store->second->setParse(true);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(e.what() + std::string(": ") + text);
    }
}