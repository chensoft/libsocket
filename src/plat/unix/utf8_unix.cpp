/**
 * Created by Jian Chen
 * @since  2016.05.31
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/utf8.hpp>

using namespace chen;

// -----------------------------------------------------------------------------
// utf8
std::string utf8::convert(std::uint32_t ch)
{
    // I want to use C++11's codecvt to do the convert
    // but unfortunately the cocos2d-x didn't support it on Android now
    // I don't like to import any third-party library to do the simple convert
    // so I bought from http://stackoverflow.com/questions/4607413/c-library-to-convert-unicode-code-points-to-utf8
    // thanks to its author
    std::string ret;

    if (ch < 0x80)
    {
        ret += ch;
    }
    else if (ch < 0x800)
    {
        ret += 192 + ch / 64;
        ret += 128 + ch % 64;
    }
    else if (ch - 0xd800u < 0x800)
    {
        // error
        return "";
    }
    else if (ch < 0x10000)
    {
        ret += 224 + ch / 4096;
        ret += 128 + ch / 64 % 64;
        ret += 128 + ch % 64;
    }
    else if (ch < 0x110000)
    {
        ret += 240 + ch / 262144;
        ret += 128 + ch / 4096 % 64;
        ret += 128 + ch / 64 % 64;
        ret += 128 + ch % 64;
    }
    else
    {
        // error
        return "";
    }

    return ret;
}