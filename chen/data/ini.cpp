/**
 * Created by Jian Chen
 * @since  2016.03.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "ini.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// ini

// dump
std::string chen::ini::dump(const chen::ini::section_type &map)
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