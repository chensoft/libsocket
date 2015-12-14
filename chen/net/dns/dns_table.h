/**
 * Created by Jian Chen
 * @since  2015.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_record.h"
#include <functional>
#include <memory>
#include <map>

namespace chen
{
    namespace dns
    {
        class table
        {
        public:
            typedef std::shared_ptr<chen::dns::RR> rr_pointer;
            typedef rr_pointer (*rr_build_type)();

        public:
            /**
             * Get table value
             */
            static std::string rrTypeToName(chen::dns::RRType key);
            static chen::dns::RRType rrNameToType(const std::string &key);
            static chen::dns::table::rr_build_type rrTypeToBuild(chen::dns::RRType key);

        public:
            /**
             * Set custom rr type and related value
             */
            static void set(chen::dns::RRType key, const std::string &val);
            static void set(chen::dns::RRType key, rr_build_type val);

        private:
            static std::map<chen::dns::RRType, std::string> _rr_name;
            static std::map<std::string, chen::dns::RRType> _rr_reverse;
            static std::map<chen::dns::RRType, rr_build_type> _rr_build;
        };
    }
}