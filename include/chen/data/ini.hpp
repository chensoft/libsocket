/**
 * A tiny ini parser
 * @since  2016.03.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   https://en.wikipedia.org/wiki/INI_file
 * -----------------------------------------------------------------------------
 * Ini file consists of two parts: section and property
 * -) Case sensitive: e.g: key=val, Key=val are two different properties
 * -) Comments: support semicolons ';', comment can appear anywhere
 * -) Line break: using '\n' in file
 * -) Duplicate key: will trigger a error, don't allow this
 * -) Escape: '\\', '\0', '\a', '\b', '\t', '\r', '\n', '\;', '\#', '\=', '\:', '\x????', '\"'
 * -) Global key: allowed this, its section name will be empty
 * -) Hierarchy: now allowed
 * -) Quoted values: allow double quotes, will remove it automatically
 * -) Whitespace: left and right whitespaces are removed automatically, add double quote if you want preserved
 * -----------------------------------------------------------------------------
 * Usage:
 * >> auto ini = chen::ini::parse("[section]\nk=v");
 * >> chen::ini::dump(ini);
 */
#pragma once

#include <string.h>
#include <unordered_map>
#include <exception>
#include <cstdlib>
#include <locale>
#include <string>
#include <chen/base/utf8.hpp>
#include <chen/base/str.hpp>

namespace chen
{
    class ini
    {
    public:
        typedef std::unordered_map<std::string, std::string> property_type;
        typedef std::unordered_map<std::string, property_type> value_type;
        typedef std::pair<std::string, property_type> section_type;

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
         * Ini parse and stringify
         */
        static chen::ini::value_type parse(const std::string &text, bool file = false);
        static std::string stringify(const chen::ini::value_type &map);

        /**
         * Decode ini, throw exception if found error
         */
        template <typename InputIterator>
        static chen::ini::value_type decode(InputIterator cur, InputIterator end);

    protected:
        /**
         * Throw syntax exception
         */
        template <typename InputIterator>
        static void exception(InputIterator &cur, InputIterator &end);

        /**
         * Decode specific type
         */
        template <typename InputIterator>
        static chen::ini::section_type decodeSection(InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static chen::ini::property_type decodeProperty(InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static std::string decodeString(InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decodeComment(InputIterator &cur, InputIterator &end);
    };
}

// decode
template <typename InputIterator>
chen::ini::value_type chen::ini::decode(InputIterator cur, InputIterator end)
{
    chen::ini::value_type value;
    bool root = true;

    while (cur != end)
    {
        for (; (cur != end) && std::isspace(*cur); ++cur)
            ;

        if (cur == end)
            break;

        switch (*cur)
        {
            case '[':
            {
                root = false;
                auto section = chen::ini::decodeSection(cur, end);

                if (value.find(section.first) == value.end())
                    value.insert(std::move(section));
                else
                    chen::ini::exception(cur, end);
            }
                break;

            case ';':
                chen::ini::decodeComment(cur, end);
                break;

            default:
                if (root)
                    value.emplace("", std::move(chen::ini::decodeProperty(cur, end)));
                else
                    chen::ini::exception(cur, end);
                break;
        }
    }

    return value;
}

// exception
template <typename InputIterator>
void chen::ini::exception(InputIterator &cur, InputIterator &end)
{
    // todo report line and column
    if (cur == end)
        throw error("ini: unexpected end of input");
    else
        throw error(str::format("ini: unexpected token '%c'", *cur));
}

// decode
template <typename InputIterator>
chen::ini::section_type chen::ini::decodeSection(InputIterator &cur, InputIterator &end)
{
    chen::ini::section_type section;

    if ((cur == end) || (*cur != '['))
        chen::ini::exception(cur, end);
    else
        ++cur;

    // name
    std::string name;

    while ((cur != end) && (*cur != ']') && (*cur != '\n'))
        name += *cur++;

    if (name.empty() || (cur == end) || (*cur != ']'))
        chen::ini::exception(cur, end);
    else
        ++cur;

    section.first = std::move(name);

    // properties
    for (; (cur != end) && std::isspace(*cur); ++cur)
        ;

    section.second = std::move(chen::ini::decodeProperty(cur, end));

    return section;
}

template <typename InputIterator>
chen::ini::property_type chen::ini::decodeProperty(InputIterator &cur, InputIterator &end)
{
    chen::ini::property_type property;

    while (cur != end)
    {
        // skip
        for (; (cur != end) && std::isspace(*cur); ++cur)
            ;

        if ((cur == end) || (*cur == '['))
            break;

        std::string key;
        std::string val;

        // key
        while ((cur != end) && (*cur != '=') && (*cur != '\n') && (*cur != ';'))
            key += *cur++;

        str::trim(key);

        if (key.empty())
            chen::ini::exception(cur, end);

        // equal
        if ((cur == end) || (*cur != '='))
            chen::ini::exception(cur, end);
        else
            ++cur;

        // val
        for (; (cur != end) && std::isspace(*cur) && (*cur != '\n'); ++cur)
            ;

        if ((cur != end) && (*cur != '\n'))
            val = std::move(chen::ini::decodeString(cur, end));

        // store
        if (property.find(key) == property.end())
            property.emplace(std::move(key), std::move(val));
        else
            throw error("ini: duplicate key found: " + key);

        // skip
        if ((cur != end) && (*cur == '\n'))
            ++cur;
    }

    return property;
}

template <typename InputIterator>
std::string chen::ini::decodeString(InputIterator &cur, InputIterator &end)
{
    std::string ret;
    bool quote = false;

    if ((cur == end) || std::isspace(*cur))
    {
        chen::ini::exception(cur, end);
    }
    else if (*cur == '"')
    {
        quote = true;
        ++cur;
    }

    while ((cur != end) && (*cur != '"') && (*cur != '\n'))
    {
        if (!quote && (*cur == ';'))
        {
            chen::ini::decodeComment(cur, end);
            break;
        }
        else if (*cur != '\\')
        {
            ret += *cur++;
            continue;
        }

        // escape
        if (++cur == end)
            chen::ini::exception(cur, end);

        switch (*cur)
        {
            case '\\':
                ret += '\\';
                break;

            case '0':
                ret += '\0';
                break;

            case 'a':
                ret += '\a';
                break;

            case 'b':
                ret += '\b';
                break;

            case 't':
                ret += '\t';
                break;

            case 'r':
                ret += '\r';
                break;

            case 'n':
                ret += '\n';
                break;

            case ';':
                ret += ';';
                break;

            case '#':
                ret += '#';
                break;

            case ':':
                ret += ':';
                break;

            case '"':
                ret += '\"';
                break;

            case 'x':
            {
                ++cur;

                // handle unicode character
                char unicode[5] = {0};

                for (auto i = 0; i < 4; ++i, ++cur)
                {
                    if (cur == end)
                        chen::ini::exception(cur, end);

                    auto ch = *cur;

                    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
                        unicode[i] = ch;  // char must in range of '0' ~ '9', 'a' ~ 'f', 'A' ~ 'F'
                    else
                        chen::ini::exception(cur, end);
                }

                // convert utf-16 to utf-8
                try
                {
                    ret.append(chen::utf8::convert(static_cast<std::uint32_t>(std::strtol(unicode, nullptr, 16))));
                }
                catch (...)
                {
                    // e.g: \xD83D\xDE00, it's a emoji character
                    throw error(str::format("ini: invalid unicode char \\x%s", unicode));
                }
            }
                break;

            default:
                chen::ini::exception(cur, end);
                break;
        }

        ++cur;
    }

    if (quote)
    {
        if ((cur == end) || (*cur != '"'))
            chen::ini::exception(cur, end);
        else
            ++cur;
    }
    else
    {
        str::rtrim(ret);
    }

    return ret;
}

template <typename InputIterator>
void chen::ini::decodeComment(InputIterator &cur, InputIterator &end)
{
    if (*cur != ';')
        chen::ini::exception(cur, end);

    for (; (cur != end) && (*cur != '\n'); ++cur)
        ;
}