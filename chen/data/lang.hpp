/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>

// todo no implement now
namespace chen
{
    class lang
    {
    public:
        static lang& standard();

    public:
        lang() = default;
        virtual ~lang() = default;

    public:
        /**
         * Translate text to another language
         */
        virtual std::string translate(const std::string &text);
    };
}

// using helper _T to wrap a text that need to translate
inline std::string _T(const std::string &text)
{
    return chen::lang::standard().translate(text);
}