/**
 * Created by Jian Chen
 * @since  2016.03.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/data/ini.hpp>
#include <chen/base/utf8.hpp>
#include <chen/base/str.hpp>
#include <chen/sys/sys.hpp>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <locale>

// -----------------------------------------------------------------------------
// ini
chen::ini::value_type chen::ini::parse(const std::string &text, bool file)
{
    if (file)
    {
        try
        {
            std::ifstream stream;
            stream.exceptions(std::ios::badbit | std::ios::failbit);
            stream.open(text.c_str(), std::ios_base::binary);

            std::istreambuf_iterator<char> cur(stream);
            return ini::parse(cur, std::istreambuf_iterator<char>());
        }
        catch (const ini::error&)
        {
            throw;
        }
        catch (const std::exception&)
        {
            throw ini::error(str::format("ini: decode %s: %s", text.c_str(), sys::error().c_str()));
        }
    }
    else
    {
        return ini::parse(text.begin(), text.end());
    }
}

chen::ini::value_type chen::ini::parse(iterator cur, iterator end)
{
    ini::value_type item;

    const iterator beg = cur;

    // trim left spaces
    if (!ini::advance(beg, cur, end))
        return item;

    // decode item
    ini::decode(item, beg, cur, end);

    // trim right spaces
    ini::advance(beg, cur, end);

    // should reach end
    if (cur != end)
        ini::exception(beg, cur, end);

    return item;
}

std::string chen::ini::stringify(const ini::value_type &map)
{
    std::string ret;
    std::size_t idx = 0;
    std::size_t len = map.size();

    for (auto &section : map)
    {
        // create section
        if (!section.first.empty())
            ret += "[" + section.first + "]\n";

        // create key and value
        std::size_t idx_v = 0;
        std::size_t len_v = section.second.size();

        for (auto &kv : section.second)
        {
            ret += kv.first + "=";

            auto size  = ret.size();
            auto quote = false;

            auto it = kv.second.begin();

            while (it != kv.second.end())
            {
                switch (*it)
                {
                    case '\\':
                        ret += "\\\\";
                        break;

                    case '\0':
                        ret += "\\0";
                        break;

                    case '\a':
                        ret += "\\a";
                        break;

                    case '\b':
                        ret += "\\b";
                        break;

                    case '\t':
                        ret += "\\t";
                        break;

                    case '\r':
                        ret += "\\r";
                        break;

                    case '\n':
                        ret += "\\n";
                        break;

                    case ';':
                        ret += "\\;";
                        break;

                    case '#':
                        ret += "\\#";
                        break;

                    case ':':
                        ret += "\\:";
                        break;

                    case '"':
                        ret += "\\\"";
                        quote = true;
                        break;

                    case ' ':
                        ret += ' ';
                        quote = true;
                        break;

                    default:
                        ret += *it;
                        break;
                }

                ++it;
            }

            // add double quote if value has left or right spaces
            if (quote)
            {
                ret.insert(ret.begin() + size, '\"');
                ret += '"';
            }

            if (idx_v < len_v - 1)
                ret += '\n';

            ++idx_v;
        }

        if (idx < len - 1)
            ret += "\n\n";

        ++idx;
    }

    return ret;
}

// exception
void chen::ini::exception(const iterator &beg, iterator &cur, iterator &end)
{
    if (cur == end)
    {
        throw ini::error("ini: unexpected end of input");
    }
    else
    {
        auto pos = cur.distance();
        auto tok = std::isprint(*cur) ? std::string(1, *cur) : str::format("\\x%02x", static_cast<int>(*cur));

        throw ini::error(str::format("ini: unexpected token '%s' at position %lu", tok.c_str(), pos), pos);
    }
}

// advance
bool chen::ini::advance(const iterator &beg, iterator &cur, iterator &end)
{
    // skip whitespaces
    while ((cur != end) && std::isspace(*cur))
        ++cur;

    // check if end
    return cur != end;
}

// decode
void chen::ini::decode(ini::value_type &out, const iterator &beg, iterator &cur, iterator &end)
{
    bool root = true;

    while (cur != end)
    {
        if (!ini::advance(beg, cur, end))
            break;

        switch (*cur)
        {
            case '[':
            {
                root = false;

                ini::section_type s;
                ini::decode(s, beg, cur, end);

                if (out.find(s.first) == out.end())
                    out.emplace(std::move(s));
                else
                    throw ini::error(str::format("ini: duplicate section '%s' found", s.first.c_str()));
            }
                break;

            case ';':
                ini::comment(beg, cur, end);
                break;

            default:
                if (root)
                {
                    ini::property_type p;
                    ini::decode(p, beg, cur, end);
                    out.emplace("", std::move(p));
                }
                else
                {
                    ini::exception(beg, cur, end);
                }
                break;
        }
    }
}

void chen::ini::decode(ini::section_type &out, const iterator &beg, iterator &cur, iterator &end)
{
    if ((cur == end) || (*cur != '['))
        ini::exception(beg, cur, end);
    else
        ++cur;

    // name
    std::string name;

    while ((cur != end) && (*cur != ']') && (*cur != '\n'))
        name += *cur++;

    if (name.empty() || (cur == end) || (*cur != ']'))
        ini::exception(beg, cur, end);
    else
        ++cur;

    out.first = std::move(name);

    // properties
    ini::advance(beg, cur, end);

    ini::property_type p;
    ini::decode(p, beg, cur, end);
    out.second = std::move(p);
}

void chen::ini::decode(ini::property_type &out, const iterator &beg, iterator &cur, iterator &end)
{
    while (cur != end)
    {
        // skip
        if (!ini::advance(beg, cur, end) || (*cur == '['))
            break;

        std::string key;
        std::string val;

        // key
        while ((cur != end) && (*cur != '=') && (*cur != '\n') && (*cur != ';'))
            key += *cur++;

        str::trim(key);

        if (key.empty())
        {
            if ((cur != end) && (*cur == ';'))
            {
                ini::comment(beg, cur, end);
                continue;
            }
            else
            {
                ini::exception(beg, cur, end);
            }
        }

        if (out.find(key) != out.end())
        {
            auto pos = cur.distance() - key.size();
            throw ini::error(str::format("ini: duplicate key '%s' found at position %lu", key.c_str(), pos), pos);
        }

        // equal
        if ((cur == end) || (*cur != '='))
            ini::exception(beg, cur, end);
        else
            ++cur;

        // val
        for (; (cur != end) && std::isspace(*cur) && (*cur != '\n'); ++cur)
            ;

        if ((cur != end) && (*cur != '\n'))
        {
            std::string s;
            ini::decode(s, beg, cur, end);
            val = std::move(s);
        }

        // store
        out.emplace(std::move(key), std::move(val));

        // skip
        if ((cur != end) && (*cur == '\n'))
            ++cur;
    }
}

void chen::ini::decode(std::string &out, const iterator &beg, iterator &cur, iterator &end)
{
    out.clear();

    if ((cur == end) || std::isspace(*cur))
        ini::exception(beg, cur, end);

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
            ini::comment(beg, cur, end);
            break;
        }
        else if (*cur != '\\')
        {
            out += *cur++;
            continue;
        }

        // escape
        if (++cur == end)
            ini::exception(beg, cur, end);

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
                        ini::exception(beg, cur, end);

                    auto ch = *cur;

                    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
                        unicode[i] = ch;  // char must in range of '0' ~ '9', 'a' ~ 'f', 'A' ~ 'F'
                    else
                        ini::exception(beg, cur, end);
                }

                // convert utf-16 to utf-8
                try
                {
                    out.append(utf8::convert(static_cast<std::uint32_t>(std::strtol(unicode, nullptr, 16))));
                }
                catch (...)
                {
                    // e.g: \xD83D\xDE00, it's a emoji character
                    auto pos = cur.distance() - 4;
                    throw ini::error(str::format("ini: invalid unicode char '\\u%s' at position %lu", unicode, pos), pos);
                }
            }
                break;

            default:
                ini::exception(beg, cur, end);
                break;
        }

        ++cur;
    }

    if (quote)
    {
        if ((cur == end) || (*cur != '"'))
            ini::exception(beg, cur, end);
        else
            ++cur;
    }
    else
    {
        str::rtrim(out);
    }
}

void chen::ini::comment(const iterator &beg, iterator &cur, iterator &end)
{
    if ((cur == end) || (*cur != ';'))
        ini::exception(beg, cur, end);

    while ((cur != end) && (*cur != '\n'))
        ++cur;
}