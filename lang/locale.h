/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <string>

// todo no implement now

// using macro _T to wrap a text that need to translate
#define _T(TEXT) (chen::locale::standard().translate(TEXT))

namespace chen
{
    class locale
    {
    public:
        static locale& standard();

    public:
        /**
         * Translate text to another language
         */
        std::string translate(const std::string &text);
    };
}