/**
 * Created by Jian Chen
 * @since  2015.10.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <string>

namespace chen
{
    namespace dns
    {
        class tool
        {
        public:
            /**
             * Check if the name is a fully qualified domain name
             */
            static bool isFqdn(const std::string &name);

            /**
             * Get the fully qualified domain name
             */
            static std::string fqdn(const std::string &name);
        };
    }
}