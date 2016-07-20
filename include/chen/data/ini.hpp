/**
 * A tiny ini parser
 * @since  2016.03.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   https://en.wikipedia.org/wiki/INI_file
 * -----------------------------------------------------------------------------
 * Ini file consists of two parts: section and property
 * -) Case sensitive: e.g: key=val, Key=val are two different properties
 * -) Duplicate key: will trigger a error, don't allow this
 * -) Global key: allowed this, its section name will be empty
 * -) Comments: support semicolons ';', comment can appear anywhere
 * -) Hierarchy: now allowed
 * -) Line break: using '\n' in file
 * -) Escape: '\\', '\0', '\a', '\b', '\t', '\r', '\n', '\;', '\#', '\=', '\:', '\x????', '\"'
 * -) Quoted values: allow double quotes, will remove it automatically
 * -) Whitespace: left and right whitespaces are removed automatically, add double quote if you want preserved
 * -----------------------------------------------------------------------------
 * Usage:
 * >> auto ini = chen::ini::parse("[section]\nk=v");
 * >> chen::ini::dump(ini);
 */
#pragma once

#include <chen/base/iterator.hpp>
#include <unordered_map>
#include <exception>

namespace chen
{
    class ini
    {
    public:
        typedef chen::position_iterator<const char, const char> iterator;  // it's a input iterator

        typedef std::unordered_map<std::string, std::string> property_type;  // k/v pair
        typedef std::pair<std::string, property_type> section_type;          // each section
        typedef std::unordered_map<std::string, property_type> value_type;   // multiple sections

        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : std::runtime_error(what) {}
        };

    public:
        ini() = default;
        virtual ~ini() = default;

    public:
        /**
         * Ini parse, accept text, file or even iterators
         * accept any iterator that satisfy the requirement of the input iterator
         */
        static chen::ini::value_type parse(const std::string &text, bool file = false);
        static chen::ini::value_type parse(iterator cur, iterator end);

        /**
         * Ini stringify
         */
        static std::string stringify(const chen::ini::value_type &map);

    protected:
        /**
         * Throw syntax exception
         */
        static void exception(const iterator &beg, iterator &cur, iterator &end);

        /**
         * Advance to the next non-whitespace character
         */
        static bool advance(const iterator &beg, iterator &cur, iterator &end);

        /**
         * Decode specific type
         */
        static void decode(chen::ini::value_type &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(chen::ini::section_type &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(chen::ini::property_type &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(std::string &out, const iterator &beg, iterator &cur, iterator &end);

        static void comment(const iterator &beg, iterator &cur, iterator &end);
    };
}