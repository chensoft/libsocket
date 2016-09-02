/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/base_error.hpp>

namespace chen
{
    namespace ip
    {
        // ---------------------------------------------------------------------
        // Syntax error
        class syntax_error : public error
        {
        public:
            explicit syntax_error(const std::string &what) : error(what) {}
        };
    }
}