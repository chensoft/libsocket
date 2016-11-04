/**
 * Created by Jian Chen
 * Fields are little-endian, they will be convert to network byte order when encoding
 * @since  2016.07.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/dns/dns_define.hpp>
#include <socket/net/ip/ip_address.hpp>
#include <memory>

namespace chen
{
    namespace dns
    {
        class encoder;
        class decoder;

        namespace edns0
        {
            // -----------------------------------------------------------------
            // opt(rfc6891, section 6.1.2)
            class opt
            {
            public:
                explicit opt(OptCode code);
                virtual ~opt() = default;

            public:
                /**
                 * Text description
                 */
                virtual std::string str(const std::string &sep = " ") const = 0;

                /**
                 * Clone current option
                 */
                virtual std::shared_ptr<opt> clone() const = 0;

            public:
                /**
                 * Encode & Decode
                 */
                void encode(dns::encoder &encoder) const;
                void decode(dns::decoder &decoder);

                static std::shared_ptr<opt> create(dns::decoder &decoder);

            protected:
                /**
                 * Pack & Unpack
                 */
                virtual void pack(dns::encoder &encoder) const = 0;
                virtual void unpack(dns::decoder &decoder) = 0;

            protected:
                /**
                 * Allow copy in derived class
                 */
                opt(const opt&) = default;
                opt& operator=(const opt&) = default;

            public:
                OptCode code = OptNone;

            protected:
                std::uint16_t length = 0;  // only valid in decoding
            };


            // -----------------------------------------------------------------
            // LLQ(http://files.dns-sd.org/draft-sekar-dns-llq.txt, section 3.2)
            class opt_llq : public opt
            {
            public:
                opt_llq();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::uint16_t version    = 0;  // Version of LLQ protocol implemented
                std::uint16_t llq_opcode = 0;  // Identifies LLQ operation
                std::uint16_t error_code = 0;  // Identifies LLQ errors
                std::uint64_t llq_id     = 0;  // Identifier for an LLQ
                std::uint32_t lease_life = 0;  // Requested or granted life of LLQ, in seconds
            };


            // -----------------------------------------------------------------
            // UL(http://files.dns-sd.org/draft-sekar-dns-ul.txt, section 5)
            class opt_ul : public opt
            {
            public:
                opt_ul();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::int32_t lease = 0;  // desired lease (request) or granted lease (response), in seconds
            };


            // -----------------------------------------------------------------
            // NSID(rfc5001, section 2.3)
            class opt_nsid : public opt
            {
            public:
                opt_nsid();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::vector<std::uint8_t> data;
            };


            // -----------------------------------------------------------------
            // DAU(rfc6975, section 3)
            class opt_dau : public opt
            {
            public:
                opt_dau();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::vector<std::uint8_t> alg_code;  // the list of assigned values of DNSSEC zone signing algorithms
            };


            // -----------------------------------------------------------------
            // DHU(rfc6975, section 3)
            class opt_dhu : public opt
            {
            public:
                opt_dhu();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::vector<std::uint8_t> alg_code;  // the list of assigned values of DNSSEC zone signing algorithms
            };


            // -----------------------------------------------------------------
            // N3U(rfc6975, section 3)
            class opt_n3u : public opt
            {
            public:
                opt_n3u();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::vector<std::uint8_t> alg_code;  // the list of assigned values of DNSSEC zone signing algorithms
            };


            // -----------------------------------------------------------------
            // Client Subnet(rfc7871, section 6)
            class opt_subnet : public opt
            {
            public:
                opt_subnet();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::uint16_t family = 0;  // address family
                std::uint8_t  source = 0;  // source prefix length
                std::uint8_t   scope = 0;  // scope prefix length
                ip::address address;       // IPv4 or IPv6 subnet
            };


            // -----------------------------------------------------------------
            // EXPIRE(rfc7314, section 3.1)
            class opt_expire : public opt
            {
            public:
                opt_expire();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::int32_t expire = 0;  // expire value, in seconds
            };


            // -----------------------------------------------------------------
            // COOKIE(rfc7873, section 4)
            class opt_cookie : public opt
            {
            public:
                opt_cookie();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::vector<std::uint8_t> client_cookie;  // fixed 8 bytes
                std::vector<std::uint8_t> server_cookie;  // empty if no server cookie, otherwise, 8 to 32 bytes
            };


            // -----------------------------------------------------------------
            // Keepalive(rfc7828, section 3.1)
            class opt_keepalive : public opt
            {
            public:
                opt_keepalive();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::uint16_t timeout = 0;  // an idle timeout value for the TCP connection, specified in units of 100 milliseconds
            };


            // -----------------------------------------------------------------
            // Padding(rfc7830, section 3)
            class opt_padding : public opt
            {
            public:
                opt_padding();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::vector<std::uint8_t> padding;
            };


            // -----------------------------------------------------------------
            // CHAIN(rfc7901, section 4)
            class opt_chain : public opt
            {
            public:
                opt_chain();

            public:
                virtual std::string str(const std::string &sep = " ") const override;
                virtual std::shared_ptr<opt> clone() const override;

            protected:
                virtual void pack(dns::encoder &encoder) const override;
                virtual void unpack(dns::decoder &decoder) override;

            public:
                std::string point;  // Closest trust point, a variable-length Fully-Qualified Domain Name(FQDN) in DNS wire format of the requested start point of the chain
            };
        }
    }
}