/**
 * Created by Jian Chen
 * @since  2016.07.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/ip/ip_address.hpp>
#include "dns_define.hpp"
#include <vector>

namespace chen
{
    namespace dns
    {
        class encoder;
        class decoder;

        namespace edns0
        {
            // -----------------------------------------------------------------
            // Option(rfc6891, section 6.1.2)
            class Option
            {
            public:
                explicit Option(OptionCode code);
                virtual ~Option() = default;

            public:
                /**
                 * Text description
                 */
                virtual std::string str(const std::string &sep = " ") const = 0;

                /**
                 * Clone current option
                 */
                virtual std::shared_ptr<chen::dns::edns0::Option> clone() const = 0;

            public:
                /**
                 * Encode & Decode
                 */
                void encode(chen::dns::encoder &encoder) const;
                void decode(chen::dns::decoder &decoder);

                static std::shared_ptr<chen::dns::edns0::Option> create(chen::dns::decoder &decoder);

            protected:
                /**
                 * Pack & Unpack
                 */
                virtual void pack(chen::dns::encoder &encoder) const = 0;
                virtual void unpack(chen::dns::decoder &decoder) = 0;

            public:
                OptionCode code = OptionCode::None;

            protected:
                std::uint16_t length = 0;  // only valid in decoding
            };


            // -----------------------------------------------------------------
            // Client Subnet(rfc7871)
            class Subnet : public Option
            {
            public:
                Subnet();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<chen::dns::edns0::Option> clone() const override;

            protected:
                /**
                 * Pack & Unpack
                 */
                virtual void pack(chen::dns::encoder &encoder) const override;
                virtual void unpack(chen::dns::decoder &decoder) override;

            public:
                std::uint16_t family = 0;  // address family
                std::uint8_t  source = 0;  // source prefix length
                std::uint8_t   scope = 0;  // scope prefix length
                std::shared_ptr<chen::ip::address> address;  // IPv4 or IPv6 address
            };
        }
    }
}