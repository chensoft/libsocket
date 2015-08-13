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

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <string>
#include <memory>
#include <limits>
#include <map>

namespace chen
{
    class cmd_option_base;

    /**
     * Command line parser
     */
    class cmd
    {
    public:
        cmd() = default;
        ~cmd() = default;

    public:
        /**
         * Define an option with default value
         */
        template <typename T>
        void define(const std::string &name,
                    const std::string &tiny = "",
                    const T &def = T(),
                    const std::string &desc = "");

    public:
        /**
         * Parse command line argv
         */
        void parse(int argc, const char *const argv[]);

        /**
         * Print options info
         */
        std::string usage();

        /**
         * Clear all options
         */
        void clear();

    protected:
        /**
         * Update value in store
         */
        void update(const std::string &text, const std::string &name, const std::string &value);

    public:
        /**
         * Get value of an option
         */
        template <typename T>
        T get(const std::string &name) const;

    protected:
        std::map<std::string, std::unique_ptr<cmd_option_base>> _store;
        std::map<std::string, std::string> _tiny;
    };

    /**
     * Option base class (internal use)
     */
    class cmd_option_base
    {
        friend class cmd;

    protected:
        cmd_option_base(const std::string &name,
                        const std::string &tiny,
                        const std::string &desc)
                : _name(name)
                , _tiny(tiny)
                , _desc(desc)
        {
        }

    public:
        virtual ~cmd_option_base() = default;

    public:
        std::string name() const;
        std::string tiny() const;
        std::string desc() const;

    public:
        virtual void setParse(bool parse);
        virtual void setValue(const std::string &value) = 0;

    private:
        cmd_option_base() = delete;

    protected:
        std::string _name;
        std::string _tiny;
        std::string _desc;

        bool _parse = false;
    };


    /**
     * Option template class (internal use)
     */
    template <typename T>
    class cmd_option : public cmd_option_base
    {
        friend class cmd;

    protected:
        cmd_option(const std::string &name,
                   const std::string &tiny,
                   const T &def,
                   const std::string &desc)
                : cmd_option_base(name, tiny, desc)
                , _def(def)
                , _val(T())
        {

        }

    public:
        virtual void setValue(const std::string &value) override;

    public:
        T def() const;
        T val() const;

    private:
        cmd_option() = delete;

    private:
        T _def;
        T _val;
    };
}


namespace chen
{
    /**
     * Option
     */
    template <typename T>
    T cmd_option<T>::def() const
    {
        return this->_def;
    }

    template <typename T>
    T cmd_option<T>::val() const
    {
        // if user provided option (even no argument), should use val, otherwise use def
        return this->_parse ? this->_val : this->_def;
    }

    /**
     * Option specialization
     */
    template <>
    inline void cmd_option<bool>::setValue(const std::string &value)
    {
        // The following options are considered true
        // program -h
        // program -h=true
        if (value.empty() || (value == "true"))
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
            val = std::stoi(value);
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
            this->_val = std::stoi(value);
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
            this->_val = std::stol(value);
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
            this->_val = std::stoll(value);
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
            val = std::stoul(value);
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
            this->_val = std::stoul(value);
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
            this->_val = std::stoull(value);
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
            this->_val = std::stof(value);
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
            this->_val = std::stod(value);
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
            this->_val = std::stold(value);
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
            this->_val = char();
        else if (value.size() == 1)
            this->_val = value[0];
        else
            throw std::invalid_argument("option: value invalid, require only one char");
    }

    template <>
    inline void cmd_option<std::string>::setValue(const std::string &value)
    {
        this->_val = value;
    }

    /**
     * Cmd
     */
    template <typename T>
    void cmd::define(const std::string &name,
                     const std::string &tiny,
                     const T &def,
                     const std::string &desc)
    {
        // name can't be null
        if (name.empty())
            throw std::invalid_argument("cmd: option is null");

        // name must be unique
        if (this->_store.count(name))
            throw std::runtime_error("cmd: option is not unique: " + name);

        // tiny must be unique
        if (!tiny.empty() && this->_tiny.count(tiny))
            throw std::runtime_error("cmd: tiny is not unique: " + tiny + ", name: " + name);

        // insert into store
        this->_store[name] = std::unique_ptr<cmd_option_base>(new cmd_option<T>(name, tiny, def, desc));

        // set tiny name
        if (!tiny.empty())
            this->_tiny[tiny] = name;
    }

    template <typename T>
    T cmd::get(const std::string &name) const
    {
        std::string tiny;

        auto it_tiny = this->_tiny.find(name);

        if (it_tiny != this->_tiny.end())
            tiny = it_tiny->second;
        else
            tiny = name;

        auto it_store = this->_store.find(tiny);

        if (it_store == this->_store.end())
            throw std::runtime_error("cmd: option not defined: " + tiny);

        auto *p = dynamic_cast<cmd_option<T> *>(it_store->second.get());

        if (!p)
            throw std::runtime_error("cmd: option defined but type is wrong: " + tiny);

        return p->val();
    }
}