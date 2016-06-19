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

#include <unordered_map>
#include <exception>
#include <cstdlib>
#include <cctype>
#include <locale>
#include <string>
#include <chen/base/utf8.hpp>
#include <chen/base/str.hpp>
#include <chen/base/num.hpp>

namespace chen
{
    class ini
    {
    public:
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
         */
        static chen::ini::value_type parse(const std::string &text, bool file = false);

        template <typename InputIterator>
        static chen::ini::value_type parse(InputIterator cur, InputIterator end);

        /**
         * Ini stringify
         */
        static std::string stringify(const chen::ini::value_type &map);

    protected:
        /**
         * Throw syntax exception
         */
        template <typename InputIterator>
        static void exception(const InputIterator &beg, InputIterator &cur, InputIterator &end);

        /**
         * Advance to the next non-whitespace character
         */
        template <typename InputIterator>
        static bool advance(const InputIterator &beg, InputIterator &cur, InputIterator &end, bool require = true);

        /**
         * Decode specific type
         */
        template <typename InputIterator>
        static void decode(chen::ini::value_type &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(chen::ini::section_type &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(chen::ini::property_type &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(std::string &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void comment(const InputIterator &beg, InputIterator &cur, InputIterator &end);
    };
}

// parse
template <typename InputIterator>
chen::ini::value_type chen::ini::parse(InputIterator cur, InputIterator end)
{
    chen::ini::value_type item;

    const InputIterator beg = cur;

    // trim left spaces
    if (!chen::ini::advance(beg, cur, end, false))
        return item;

    // decode item
    chen::ini::decode(item, beg, cur, end);

    // trim right spaces
    chen::ini::advance(beg, cur, end, false);

    // should reach end
    if (cur != end)
        chen::ini::exception(beg, cur, end);

    return item;
}

// exception
template <typename InputIterator>
void chen::ini::exception(const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if (cur == end)
    {
        throw error("ini: unexpected end of input");
    }
    else
    {
        auto pos = chen::num::str(std::distance(beg, cur));
        throw error(chen::str::format("ini: unexpected token '%c' at position %s", *cur, pos.c_str()));
    }
}

// advance
template <typename InputIterator>
bool chen::ini::advance(const InputIterator &beg, InputIterator &cur, InputIterator &end, bool require)
{
    // skip whitespaces
    while ((cur != end) && std::isspace(*cur))
        ++cur;

    // check if end
    if (cur == end)
    {
        if (require)
            chen::ini::exception(beg, cur, end);
        else
            return false;
    }

    return true;
}

// decode
template <typename InputIterator>
void chen::ini::decode(chen::ini::value_type &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    bool root = true;

    while (cur != end)
    {
        if (!chen::ini::advance(beg, cur, end, false))
            break;

        switch (*cur)
        {
            case '[':
            {
                root = false;

                chen::ini::section_type s;
                chen::ini::decode(s, beg, cur, end);

                if (out.find(s.first) == out.end())
                    out.emplace(std::move(s));
                else
                    chen::ini::exception(beg, cur, end);
            }
                break;

            case ';':
                chen::ini::comment(beg, cur, end);
                break;

            default:
                if (root)
                {
                    chen::ini::property_type p;
                    chen::ini::decode(p, beg, cur, end);
                    out.emplace("", std::move(p));
                }
                else
                {
                    chen::ini::exception(beg, cur, end);
                }
                break;
        }
    }
}

template <typename InputIterator>
void chen::ini::decode(chen::ini::section_type &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if ((cur == end) || (*cur != '['))
        chen::ini::exception(beg, cur, end);
    else
        ++cur;

    // name
    std::string name;

    while ((cur != end) && (*cur != ']') && (*cur != '\n'))
        name += *cur++;

    if (name.empty() || (cur == end) || (*cur != ']'))
        chen::ini::exception(beg, cur, end);
    else
        ++cur;

    out.first = std::move(name);

    // properties
    chen::ini::advance(beg, cur, end, false);

    chen::ini::property_type p;
    chen::ini::decode(p, beg, cur, end);
    out.second = std::move(p);
}

template <typename InputIterator>
void chen::ini::decode(chen::ini::property_type &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    while (cur != end)
    {
        // skip
        if (!chen::ini::advance(beg, cur, end, false) || (*cur == '['))
            break;

        std::string key;
        std::string val;

        // key
        while ((cur != end) && (*cur != '=') && (*cur != '\n') && (*cur != ';'))
            key += *cur++;

        str::trim(key);

        if (key.empty())
            chen::ini::exception(beg, cur, end);

        // equal
        if ((cur == end) || (*cur != '='))
            chen::ini::exception(beg, cur, end);
        else
            ++cur;

        // val
        for (; (cur != end) && std::isspace(*cur) && (*cur != '\n'); ++cur)
            ;

        if ((cur != end) && (*cur != '\n'))
        {
            std::string s;
            chen::ini::decode(s, beg, cur, end);
            val = std::move(s);
        }

        // store
        if (out.find(key) == out.end())
        {
            out.emplace(std::move(key), std::move(val));
        }
        else
        {
            auto pos = chen::num::str(std::distance(beg, cur) - 4);
            throw error(chen::str::format("ini: duplicate key %s found at position %s", key.c_str(), pos.c_str()));
        }

        // skip
        if ((cur != end) && (*cur == '\n'))
            ++cur;
    }
}

template <typename InputIterator>
void chen::ini::decode(std::string &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if ((cur == end) || std::isspace(*cur))
        chen::ini::exception(beg, cur, end);

    bool quote = false;

    if (*cur == '"')
    {
        quote = true;
        ++cur;
    }

    while ((cur != end) && (*cur != '"') && (*cur != '\n'))
    {
        if (!quote && (*cur == ';'))
        {
            chen::ini::comment(beg, cur, end);
            break;
        }
        else if (*cur != '\\')
        {
            out += *cur++;
            continue;
        }

        // escape
        if (++cur == end)
            chen::ini::exception(beg, cur, end);

        switch (*cur)
        {
            case '\\':
                out += '\\';
                break;

            case '0':
                out += '\0';
                break;

            case 'a':
                out += '\a';
                break;

            case 'b':
                out += '\b';
                break;

            case 't':
                out += '\t';
                break;

            case 'r':
                out += '\r';
                break;

            case 'n':
                out += '\n';
                break;

            case ';':
                out += ';';
                break;

            case '#':
                out += '#';
                break;

            case ':':
                out += ':';
                break;

            case '"':
                out += '\"';
                break;

            case 'x':
            {
                ++cur;

                // handle unicode character
                char unicode[5];
                unicode[4] = '\0';

                for (auto i = 0; i < 4; ++i, ++cur)
                {
                    if (cur == end)
                        chen::ini::exception(beg, cur, end);

                    auto ch = *cur;

                    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
                        unicode[i] = ch;  // char must in range of '0' ~ '9', 'a' ~ 'f', 'A' ~ 'F'
                    else
                        chen::ini::exception(beg, cur, end);
                }

                // convert utf-16 to utf-8
                try
                {
                    out.append(chen::utf8::convert(static_cast<std::uint32_t>(std::strtol(unicode, nullptr, 16))));
                }
                catch (...)
                {
                    // e.g: \xD83D\xDE00, it's a emoji character
                    auto pos = chen::num::str(std::distance(beg, cur) - 4);
                    throw error(chen::str::format("ini: invalid unicode char \\u%s at position %s", unicode, pos.c_str()));
                }
            }
                break;

            default:
                chen::ini::exception(beg, cur, end);
                break;
        }

        ++cur;
    }

    if (quote)
    {
        if ((cur == end) || (*cur != '"'))
            chen::ini::exception(beg, cur, end);
        else
            ++cur;
    }
    else
    {
        str::rtrim(out);
    }
}

template <typename InputIterator>
void chen::ini::comment(const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if ((cur == end) || (*cur != ';'))
        chen::ini::exception(beg, cur, end);

    while ((cur != end) && (*cur != '\n'))
        ++cur;
}