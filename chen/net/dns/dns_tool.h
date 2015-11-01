/**
 * Created by Jian Chen
 * @since  2015.10.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <chen/net/ip/ip_addr.h>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // tool
        class tool
        {
        public:
            /**
             * Pretty print for vector
             */
            static std::string format(const std::vector<std::uint8_t> &vec);

            /**
             * Check if the name is a fully qualified domain name
             */
            static bool isFqdn(const std::string &name);

            /**
             * Get the fully qualified domain name
             */
            static std::string fqdn(const std::string &name);

            /**
             * Get local nameservers
             */
            static std::vector<chen::ip::address_v4> nameservers();
        };


        // ---------------------------------------------------------------------
        // pack
        class pack
        {
        public:
            /**
             * Name & Binary convert
             */
            static std::size_t nameToBinary(const std::string &name, std::vector<std::uint8_t> &store);
            static std::string binaryToName(const std::uint8_t *data, std::size_t size);
        };
    }
}