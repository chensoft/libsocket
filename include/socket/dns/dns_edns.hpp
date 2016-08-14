/**
 * Created by Jian Chen
 * Fields are little-endian, they will be convert to network byte order when encoding
 * @since  2016.07.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_address.hpp>
#include "dns_define.hpp"

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
                virtual std::shared_ptr<edns0::Option> clone() const = 0;

            public:
                /**
                 * Encode & Decode
                 */
                void encode(encoder &encoder) const;
                void decode(decoder &decoder);

                static std::shared_ptr<edns0::Option> create(decoder &decoder);

            protected:
                /**
                 * Pack & Unpack
                 */
                virtual void pack(encoder &encoder) const = 0;
                virtual void unpack(decoder &decoder) = 0;

            protected:
                /**
                 * Allow copy in derived class
                 */
                Option(const Option&) = default;
                Option& operator=(const Option&) = default;

            public:
                OptionCode code = OptionCode::None;

            protected:
                std::uint16_t length = 0;  // only valid in decoding
            };


            // -----------------------------------------------------------------
            // LLQ(http://files.dns-sd.org/draft-sekar-dns-llq.txt, section 3.2)
            class LLQ : public Option
            {
            public:
                LLQ();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::uint16_t version    = 0;  // Version of LLQ protocol implemented
                std::uint16_t llq_opcode = 0;  // Identifies LLQ operation
                std::uint16_t error_code = 0;  // Identifies LLQ errors
                std::uint64_t llq_id     = 0;  // Identifier for an LLQ
                std::uint32_t lease_life = 0;  // Requested or granted life of LLQ, in seconds
            };


            // -----------------------------------------------------------------
            // UL(http://files.dns-sd.org/draft-sekar-dns-ul.txt, section 5)
            class UL : public Option
            {
            public:
                UL();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::int32_t lease = 0;  // desired lease (request) or granted lease (response), in seconds
            };


            // -----------------------------------------------------------------
            // NSID(rfc5001, section 2.3)
            class NSID : public Option
            {
            public:
                NSID();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::vector<std::uint8_t> data;
            };


            // -----------------------------------------------------------------
            // DAU(rfc6975, section 3)
            class DAU : public Option
            {
            public:
                DAU();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::vector<std::uint8_t> alg_code;  // the list of assigned values of DNSSEC zone signing algorithms
            };


            // -----------------------------------------------------------------
            // DHU(rfc6975, section 3)
            class DHU : public Option
            {
            public:
                DHU();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::vector<std::uint8_t> alg_code;  // the list of assigned values of DNSSEC zone signing algorithms
            };


            // -----------------------------------------------------------------
            // N3U(rfc6975, section 3)
            class N3U : public Option
            {
            public:
                N3U();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::vector<std::uint8_t> alg_code;  // the list of assigned values of DNSSEC zone signing algorithms
            };


            // -----------------------------------------------------------------
            // Client Subnet(rfc7871, section 6)
            class Subnet : public Option
            {
            public:
                Subnet();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::uint16_t family = 0;  // address family
                std::uint8_t  source = 0;  // source prefix length
                std::uint8_t   scope = 0;  // scope prefix length
                net::address address;      // IPv4 or IPv6 subnet
            };


            // -----------------------------------------------------------------
            // EXPIRE(rfc7314, section 3.1)
            class EXPIRE : public Option
            {
            public:
                EXPIRE();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::int32_t expire = 0;  // expire value, in seconds
            };


            // -----------------------------------------------------------------
            // COOKIE(rfc7873, section 4)
            class COOKIE : public Option
            {
            public:
                COOKIE();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::vector<std::uint8_t> client_cookie;  // fixed 8 bytes
                std::vector<std::uint8_t> server_cookie;  // empty if no server cookie, otherwise, 8 to 32 bytes
            };


            // -----------------------------------------------------------------
            // Keepalive(rfc7828, section 3.1)
            class Keepalive : public Option
            {
            public:
                Keepalive();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::uint16_t timeout = 0;  // an idle timeout value for the TCP connection, specified in units of 100 milliseconds
            };


            // -----------------------------------------------------------------
            // Padding(rfc7830, section 3)
            class Padding : public Option
            {
            public:
                Padding();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::vector<std::uint8_t> padding;
            };


            // -----------------------------------------------------------------
            // CHAIN(rfc7901, section 4)
            class CHAIN : public Option
            {
            public:
                CHAIN();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<edns0::Option> clone() const override;

            protected:
                virtual void pack(encoder &encoder) const override;
                virtual void unpack(decoder &decoder) override;

            public:
                std::string point;  // Closest trust point, a variable-length Fully-Qualified Domain Name(FQDN) in DNS wire format of the requested start point of the chain
            };
        }
    }
}