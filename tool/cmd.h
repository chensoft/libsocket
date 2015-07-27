/**
 * Created by Jian Chen
 * @since  2015.07.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 * -----------------------------------------------------------------------------
 * Full command line format can be viewed in http://docopt.org/
 * We only implement a subset of the format
 * :-) program command
 * :-) program -abc
 * :-) program -option <argument>
 * :-) program -option=<argument>
 * :-) program --option <argument>
 * :-) program --option=<argument>
 * -----------------------------------------------------------------------------
 * Usage:
 * >> chen::cmd flag;
 * >>
 * >> flag.define<bool>("help", "h", "show help");
 * >> flag.define<int>("port", "p", "server port (default: 53)", 53);
 * >> flag.define<std::string>("zone", "z", "zone folder", "");
 * >>
 * >> flag.parse(argc, argv);
 * >>
 * >> bool help = flag.get<bool>("help");
 */
#pragma once

#include <string>
#include <map>

namespace chen
{
    /**
     * Option base class (internal use)
     */
    class cmd_option_base
    {
        friend class cmd;

    protected:
        cmd_option_base(const std::string &name,
                        const std::string &alias,
                        const std::string &desc)
        : _name(name)
        , _alias(alias)
        , _desc(desc)
        {

        }

    public:
        virtual ~cmd_option_base()
        {

        }

    public:
        std::string name() const
        {
            return this->_name;
        }

        std::string alias() const
        {
            return this->_alias;
        }

        std::string desc() const
        {
            return this->_desc;
        }

    public:
        virtual void setValue(const std::string &value) = 0;

        void setParse(bool parse)
        {
            this->_parse = parse;
        }

    private:
        cmd_option_base() = delete;

    protected:
        std::string _name;
        std::string _alias;
        std::string _desc;

        bool _parse = false;
    };


    /**
     * Option template class (internal use)
     */
    template <class T>
    class cmd_option : public cmd_option_base
    {
        friend class cmd;

    protected:
        cmd_option(const std::string &name,
                   const std::string &alias,
                   const std::string &desc,
                   const T &def,
                   bool must)
        : cmd_option_base(name, alias, desc)
        , _def(def)
        , _val(T())
        , _must(must)
        {

        }

    public:
        virtual void setValue(const std::string &value) override;

    public:
        T def() const
        {
            return this->_def;
        }

        T val() const
        {
            return this->_parse ? this->_val : this->_def;
        }

    private:
        cmd_option() = delete;

    private:
        T _def;
        T _val;

        bool _must = false;
    };


    /**
     * Option specialization
     */
    template <>
    inline void cmd_option<bool>::setValue(const std::string &value)
    {
        // The following options are considered true
        // program -h
        // program -h=true
        if (value.empty())
            this->_val = this->_must ? this->_def : true;
        else if (value == "true")
            this->_val = true;
        else if (value == "false")
            this->_val = false;
        else
            throw std::invalid_argument("option: value invalid, require true or false");
    }

    template <>
    inline void cmd_option<short>::setValue(const std::string &value)
    {
        // check range overflow
        int val = 0;

        try
        {
            val = (value.empty() && this->_must) ? this->_def : std::stoi(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require short");
        }

        if (val > std::numeric_limits<short>::max())
            throw std::range_error("option: value large than short");
        else
            this->_val = static_cast<short>(val);
    }

    template <>
    inline void cmd_option<int>::setValue(const std::string &value)
    {
        try
        {
            this->_val = (value.empty() && this->_must) ? this->_def : std::stoi(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require int");
        }
    }

    template <>
    inline void cmd_option<long>::setValue(const std::string &value)
    {
        try
        {
            this->_val = (value.empty() && this->_must) ? this->_def : std::stol(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require long");
        }
    }

    template <>
    inline void cmd_option<long long>::setValue(const std::string &value)
    {
        try
        {
            this->_val = (value.empty() && this->_must) ? this->_def : std::stoll(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require long long");
        }
    }

    template <>
    inline void cmd_option<unsigned int>::setValue(const std::string &value)
    {
        unsigned long val = 0;

        try
        {
            val = (value.empty() && this->_must) ? this->_def : std::stoul(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require unsigned int");
        }

        if (val > std::numeric_limits<unsigned int>::max())
            throw std::range_error("option: value large than unsigned int");
        else
            this->_val = static_cast<unsigned int>(val);
    }

    template <>
    inline void cmd_option<unsigned long>::setValue(const std::string &value)
    {
        try
        {
            this->_val = (value.empty() && this->_must) ? this->_def : std::stoul(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require unsigned long");
        }
    }

    template <>
    inline void cmd_option<unsigned long long>::setValue(const std::string &value)
    {
        try
        {
            this->_val = (value.empty() && this->_must) ? this->_def : std::stoull(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require unsigned long long");
        }
    }

    template <>
    inline void cmd_option<float>::setValue(const std::string &value)
    {
        try
        {
            this->_val = (value.empty() && this->_must) ? this->_def : std::stof(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require float");
        }
    }

    template <>
    inline void cmd_option<double>::setValue(const std::string &value)
    {
        try
        {
            this->_val = (value.empty() && this->_must) ? this->_def : std::stod(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require double");
        }
    }

    template <>
    inline void cmd_option<long double>::setValue(const std::string &value)
    {
        try
        {
            this->_val = (value.empty() && this->_must) ? this->_def : std::stold(value);
        }
        catch (...)
        {
            throw std::invalid_argument("option: value invalid, require long double");
        }
    }

    template <>
    inline void cmd_option<char>::setValue(const std::string &value)
    {
        // char's type is decide by compiler, maybe signed or unsigned
        if (value.empty())
            this->_val = this->_must ? this->_def : char();
        else if (value.size() == 1)
            this->_val = value[0];
        else
            throw std::invalid_argument("option: value invalid, require only one char");
    }

    template <>
    inline void cmd_option<std::string>::setValue(const std::string &value)
    {
        this->_val = (value.empty() && this->_must) ? this->_def : value;
    }


    /**
     * Command line parser
     */
    class cmd
    {
    public:
        cmd()
        {

        }

        ~cmd()
        {
            for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
                delete it->second;
        }

    public:
        /**
         * Define an option with default value
         */
        template <class T>
        void define(const std::string &name,
                    const std::string &alias = "",
                    const std::string &desc = "",
                    const T &def = T(),
                    bool must = false)
        {
            // name can't be null
            if (name.empty())
                throw std::invalid_argument("cmd: option is null");

            // name must be unique
            if (this->_store.count(name))
                throw std::runtime_error("cmd: option is not unique: " + name);

            // alias must be unique
            if (this->_alias.count(alias))
                throw std::runtime_error("cmd: alias is not unique: " + alias + ", name: " + name);

            // insert into store
            this->_store[name] = new cmd_option<T>(name, alias, desc, def, must);

            // set alias name
            this->_alias[alias] = name;
        }

    public:
        /**
         * Parse command line argv
         */
        void parse(int argc, const char *argv[])
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
                                value = argv[idx + 1];

                                ++idx;  // already parsed
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
                                value = argv[idx + 1];

                                ++idx;  // already parsed
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
                        {
                            this->update(text, std::string(1, name[i]), "");
                        }
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

        /**
         * Clear all options
         */
        void clear()
        {
            for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
                it->second->setParse(false);
        }

    protected:
        /**
         * Update value in store
         */
        void update(const std::string &text, const std::string &name, const std::string &value)
        {
            std::string alias;

            auto it_alias = this->_alias.find(name);

            if (it_alias != this->_alias.end())
                alias = it_alias->second;
            else
                alias = name;

            auto it_store = this->_store.find(alias);

            if (it_store == this->_store.end())
                throw std::runtime_error("cmd: parse option but not defined: " + text);

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

    public:
        /**
         * Get value of an option
         */
        template <class T>
        T get(const std::string &name) const
        {
            std::string alias;

            auto it_alias = this->_alias.find(name);

            if (it_alias != this->_alias.end())
                alias = it_alias->second;
            else
                alias = name;

            auto it_store = this->_store.find(alias);

            if (it_store == this->_store.end())
                throw std::runtime_error("cmd: option not defined: " + alias);

            auto *p = dynamic_cast<cmd_option<T> *>(it_store->second);

            if (!p)
                throw std::runtime_error("cmd: option defined but type is wrong: " + alias);

            return p->val();
        }

    protected:
        std::map<std::string, cmd_option_base*> _store;
        std::map<std::string, std::string> _alias;
    };
}