/**
 * Created by Jian Chen
 * @since  2016.05.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "utf8.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// utf8
std::size_t utf8::count(const std::string &text)
{
    auto ptr = text.data();
    auto len = text.size();

    std::size_t num  = 0;
    std::size_t pos  = 0;
    std::size_t step = 0;

    while (pos < len)
    {
        if ((ptr[pos] & 0x80) == 0x00)       // 1Bytes
            step = 1;
        else if ((ptr[pos] & 0xE0) == 0xC0)  // 2Bytes
            step = 2;
        else if ((ptr[pos] & 0xF0) == 0xE0)  // 3Bytes
            step = 3;
        else if ((ptr[pos] & 0xF8) == 0xF0)  // 4Bytes
            step = 4;
        else if ((ptr[pos] & 0xFC) == 0xF8)  // 5Bytes
            step = 5;
        else if ((ptr[pos] & 0xFE) == 0xFC)  // 6Bytes
            step = 6;
        else
            break;

        ++num;
        pos += step;
    }

    return num;
}

std::string utf8::substr(const std::string &text, std::size_t start, std::size_t count)
{
    if (!count)
        return "";

    auto ptr = text.data();
    auto len = text.size();

    std::string ret;

    std::size_t num  = 0;
    std::size_t pos  = 0;
    std::size_t step = 0;

    while ((pos < len) && count)
    {
        if ((ptr[pos] & 0x80) == 0x00)       // 1Bytes
            step = 1;
        else if ((ptr[pos] & 0xE0) == 0xC0)  // 2Bytes
            step = 2;
        else if ((ptr[pos] & 0xF0) == 0xE0)  // 3Bytes
            step = 3;
        else if ((ptr[pos] & 0xF8) == 0xF0)  // 4Bytes
            step = 4;
        else if ((ptr[pos] & 0xFC) == 0xF8)  // 5Bytes
            step = 5;
        else if ((ptr[pos] & 0xFE) == 0xFC)  // 6Bytes
            step = 6;
        else
            return ret;

        if (num >= start)
        {
            ret.append(ptr + pos, step);
            count--;
        }

        ++num;
        pos += step;
    }

    return ret;
}

bool utf8::bom(const std::string &text)
{
    return (text.size() >= 3) &&
            (text[0] == static_cast<char>(0xEF)) &&
            (text[1] == static_cast<char>(0xBB)) &&
            (text[2] == static_cast<char>(0xBF));
}