/**
 * A tiny ini parser
 * @since  2016.03.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   https://en.wikipedia.org/wiki/INI_file
 * -----------------------------------------------------------------------------
 * Ini file is consists of three parts: section, key, value
 * This ini parser has the following features:
 * -) Case sensitive: e.g: key=val, Key=val are two different k/v pairs
 * -) Comments: support semicolons ';', comment can appear anywhere
 * -) Line break: using '\n' in file
 * -) Duplicate key: will trigger a error, don't allow this
 * -) Escape: '\\', '\0', '\a', '\b', '\t', '\r', '\n', '\;', '\#', '\=', '\:', '\x????', '\"'
 * -) Global key: allowed this, its section name will be empty
 * -) Hierarchy: now allowed
 * -) Quoted values: allow double quotes, will remove it automatically
 * -) Whitespace: left and right whitespaces are removed automatically, add double quote if you want preserved
 * -----------------------------------------------------------------------------
 * Usage: todo
 * >>
 */
#pragma once

#include <exception>
#include <string>
#include <map>

namespace chen
{
    class ini
    {
    public:
        typedef std::map<std::string, std::string> value_type;
        typedef std::map<std::string, value_type> section_type;

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
         * Ini parse and dump
         */
        static chen::ini::section_type parse(const std::string &text, bool file = false);
        static std::string dump(const chen::ini::section_type &map);
    };
}