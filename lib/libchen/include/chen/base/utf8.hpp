/**
 * Created by Jian Chen
 * @since  2016.05.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   https://en.wikipedia.org/wiki/UTF-8
 */
#pragma once

#include <string>

namespace chen
{
    class utf8
    {
    public:
        /**
         * Character count
         */
        static std::size_t count(const std::string &text);

        /**
         * Retrieve sub-string
         */
        static std::string substr(const std::string &text, std::size_t start, std::size_t count);

        /**
         * Check the BOM: 0xef, 0xbb, 0xbf
         */
        static bool bom(const std::string &text);

        /**
         * Convert from utf-16 to utf-8
         */
        static std::string convert(std::uint32_t ch);
    };
}