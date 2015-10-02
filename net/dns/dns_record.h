/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 * @link   https://tools.ietf.org/html/rfc1035
 */
#pragma once

#include <cstdint>
#include <string>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // Resource Record
        class RR
        {
        public:
            /**
             * Get filed value
             */
            std::string name()       const;
            std::uint16_t rrtype()   const;
            std::uint16_t rrclass()  const;
            std::int32_t ttl()       const;
            std::uint16_t rdlength() const;

            /**
             * Set filed value
             */
            void setName(const std::string &value);
            void setRrtype(std::uint16_t value);
            void setRrclass(std::uint16_t value);
            void setTtl(std::int32_t value);
            void setRdlength(std::uint16_t value);

        private:
            std::string _name;
            std::uint16_t _rrtype   = 0;
            std::uint16_t _rrclass  = 0;
            std::int32_t _ttl       = 0;
            std::uint16_t _rdlength = 0;
        };


        // ---------------------------------------------------------------------
        // A
        class A : public RR
        {
        public:

        private:
            std::string _address;
        };
    }
}