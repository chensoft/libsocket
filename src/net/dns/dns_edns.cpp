/**
 * Created by Jian Chen
 * @since  2016.07.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/dns/dns_edns.hpp>
#include <socket/net/dns/dns_table.hpp>
#include <socket/net/dns/dns_codec.hpp>
#include <chen/base/num.hpp>
#include <cmath>

// -----------------------------------------------------------------------------
// edns0 - opt
chen::dns::edns0::opt::opt(OptCode code) : code(code)
{
}

void chen::dns::edns0::opt::encode(dns::encoder &encoder) const
{
    // common
    encoder.pack(this->code);

    // length, placeholder here
    encoder.pack(static_cast<std::uint16_t>(0));

    // current size
    auto before = encoder.size();

    // encode subclass
    this->pack(encoder);

    // length
    auto len = encoder.size();
    auto off = len - before;

    if (off > (std::numeric_limits<std::uint16_t>::max)())
        throw std::runtime_error("dns: codec pack option data size is overflow");

    auto tmp = static_cast<std::uint16_t>(off);

    encoder.change(len - tmp - 2, static_cast<std::uint8_t>(tmp >> 8 & 0xFF));
    encoder.change(len - tmp - 1, static_cast<std::uint8_t>(tmp & 0xFF));
}

void chen::dns::edns0::opt::decode(dns::decoder &decoder)
{
    // unpack base fields
    decoder.unpack(this->code);
    decoder.unpack(this->length);

    // unpack subclass
    this->unpack(decoder);
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt::create(dns::decoder &decoder)
{
    dns::decoder detect(decoder);

    // detect type
    OptCode code = OptNone;
    detect.unpack(code);

    // build opt
    std::shared_ptr<edns0::opt> opt = table::build(code);
    if (!opt)
        return nullptr;

    // decode it
    opt->decode(decoder);

    return opt;
}


// -----------------------------------------------------------------------------
// edns0 - LLQ
chen::dns::edns0::opt_llq::opt_llq() : opt(OptLLQ)
{
}

std::string chen::dns::edns0::opt_llq::str(const std::string &sep) const
{
    std::string ret("LLQ");

    ret += sep + num::str(this->version);
    ret += sep + num::str(this->llq_opcode);
    ret += sep + num::str(this->error_code);
    ret += sep + num::str(this->llq_id);
    ret += sep + num::str(this->lease_life);

    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_llq::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_llq::pack(dns::encoder &encoder) const
{
    encoder.pack(this->version);
    encoder.pack(this->llq_opcode);
    encoder.pack(this->error_code);
    encoder.pack(this->llq_id);
    encoder.pack(this->lease_life);
}

void chen::dns::edns0::opt_llq::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->version);
    decoder.unpack(this->llq_opcode);
    decoder.unpack(this->error_code);
    decoder.unpack(this->llq_id);
    decoder.unpack(this->lease_life);
}


// -----------------------------------------------------------------------------
// edns0 - UL
chen::dns::edns0::opt_ul::opt_ul() : opt(OptUL)
{
}

std::string chen::dns::edns0::opt_ul::str(const std::string &sep) const
{
    std::string ret("UL");
    ret += sep + num::str(this->lease);
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_ul::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_ul::pack(dns::encoder &encoder) const
{
    encoder.pack(this->lease);
}

void chen::dns::edns0::opt_ul::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->lease);
}


// -----------------------------------------------------------------------------
// edns0 - NSID
chen::dns::edns0::opt_nsid::opt_nsid() : opt(OptNSID)
{
}

std::string chen::dns::edns0::opt_nsid::str(const std::string &sep) const
{
    std::string ret("NSID");
    ret += sep + rr::escape(this->data);
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_nsid::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_nsid::pack(dns::encoder &encoder) const
{
    encoder.pack(this->data, this->data.size());
}

void chen::dns::edns0::opt_nsid::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->data, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - DAU
chen::dns::edns0::opt_dau::opt_dau() : opt(OptDAU)
{
}

std::string chen::dns::edns0::opt_dau::str(const std::string &sep) const
{
    std::string ret("DAU");
    ret += sep + rr::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_dau::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_dau::pack(dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void chen::dns::edns0::opt_dau::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - DHU
chen::dns::edns0::opt_dhu::opt_dhu() : opt(OptDHU)
{
}

std::string chen::dns::edns0::opt_dhu::str(const std::string &sep) const
{
    std::string ret("DHU");
    ret += sep + rr::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_dhu::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_dhu::pack(dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void chen::dns::edns0::opt_dhu::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - N3U
chen::dns::edns0::opt_n3u::opt_n3u() : opt(OptN3U)
{
}

std::string chen::dns::edns0::opt_n3u::str(const std::string &sep) const
{
    std::string ret("N3U");
    ret += sep + rr::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_n3u::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_n3u::pack(dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void chen::dns::edns0::opt_n3u::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - Subnet
chen::dns::edns0::opt_subnet::opt_subnet() : opt(OptSubnet), address(chen::ip_address::Type::IPv4)
{
}

std::string chen::dns::edns0::opt_subnet::str(const std::string &sep) const
{
    std::string ret("Client Subnet");

    ret += sep + this->address.str();
    ret += "/" + num::str(this->source);
    ret += "/" + num::str(this->scope);

    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_subnet::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_subnet::pack(dns::encoder &encoder) const
{
    encoder.pack(this->family);
    encoder.pack(this->source);
    encoder.pack(this->scope);

    switch (this->family)
    {
        case 1:
        {
            // IPv4
            if (this->source > 32)
                throw std::runtime_error("dns: codec pack edns0 subnet source prefix is greater than 32");

            if (!this->address.isIPv4())
                throw std::runtime_error("dns: codec pack edns0 subnet address is not IPv4");

            auto bytes = this->address.network().bytes();

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                encoder.pack(bytes[i]);
        }
            break;

        case 2:
        {
            if (this->source > 128)
                throw std::runtime_error("dns: codec pack edns0 subnet source prefix is greater than 128");

            if (!this->address.isIPv6())
                throw std::runtime_error("dns: codec pack edns0 subnet address is not IPv6");

            auto bytes = this->address.network().bytes();

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                encoder.pack(bytes[i]);
        }
            break;

        default:
            throw std::runtime_error("dns: codec pack edns0 subnet family not support");
    }
}

void chen::dns::edns0::opt_subnet::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->family);
    decoder.unpack(this->source);
    decoder.unpack(this->scope);

    switch (this->family)
    {
        case 1:
        {
            // IPv4
            if (this->source > 32)
                throw std::runtime_error("dns: codec unpack edns0 subnet source prefix is greater than 32");

            std::uint8_t addr[4]{};

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                decoder.unpack(addr[3 - i]);

            this->address = ip_version4(*reinterpret_cast<std::uint32_t*>(addr), this->source);
        }
            break;

        case 2:
        {
            // IPv6
            if (this->source > 128)
                throw std::runtime_error("dns: codec unpack edns0 subnet source prefix is greater than 128");

            std::uint8_t addr[16]{};

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                decoder.unpack(addr[15 - i]);

            this->address = ip_version6(addr, this->source);
        }
            break;

        default:
            throw std::runtime_error("dns: codec unpack edns0 subnet family not support");
    }
}


// -----------------------------------------------------------------------------
// edns0 - EXPIRE
chen::dns::edns0::opt_expire::opt_expire() : opt(OptEXPIRE)
{
}

std::string chen::dns::edns0::opt_expire::str(const std::string &sep) const
{
    std::string ret("EXPIRE");
    ret += sep + num::str(this->expire);
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_expire::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_expire::pack(dns::encoder &encoder) const
{
    encoder.pack(this->expire);
}

void chen::dns::edns0::opt_expire::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->expire);
}


// -----------------------------------------------------------------------------
// edns0 - COOKIE
chen::dns::edns0::opt_cookie::opt_cookie() : opt(OptCOOKIE)
{
}

std::string chen::dns::edns0::opt_cookie::str(const std::string &sep) const
{
    std::string ret("COOKIE");

    if (this->server_cookie.empty())
    {
        ret += sep + rr::escape(this->client_cookie.size());
    }
    else
    {
        ret += sep + rr::escape(this->client_cookie.size());
        ret += sep + rr::escape(this->server_cookie.size());
    }

    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_cookie::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_cookie::pack(dns::encoder &encoder) const
{
    encoder.pack(this->client_cookie, this->client_cookie.size());
    encoder.pack(this->server_cookie, this->server_cookie.size());
}

void chen::dns::edns0::opt_cookie::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->client_cookie, 8);
    decoder.unpack(this->server_cookie, this->length - 8u);
}


// -----------------------------------------------------------------------------
// edns0 - Keepalive
chen::dns::edns0::opt_keepalive::opt_keepalive() : opt(OptKeepalive)
{
}

std::string chen::dns::edns0::opt_keepalive::str(const std::string &sep) const
{
    std::string ret("TCP Keepalive");
    ret += sep + num::str(this->timeout);
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_keepalive::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_keepalive::pack(dns::encoder &encoder) const
{
    encoder.pack(this->timeout);
}

void chen::dns::edns0::opt_keepalive::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->timeout);
}


// -----------------------------------------------------------------------------
// edns0 - Padding
chen::dns::edns0::opt_padding::opt_padding() : opt(OptPadding)
{
}

std::string chen::dns::edns0::opt_padding::str(const std::string &sep) const
{
    std::string ret("Padding");
    ret += sep + rr::escape(this->padding.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_padding::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_padding::pack(dns::encoder &encoder) const
{
    encoder.pack(this->padding, this->padding.size());
}

void chen::dns::edns0::opt_padding::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->padding, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - CHAIN
chen::dns::edns0::opt_chain::opt_chain() : opt(OptCHAIN)
{
}

std::string chen::dns::edns0::opt_chain::str(const std::string &sep) const
{
    std::string ret("CHAIN");
    ret += sep + this->point;
    return ret;
}

std::shared_ptr<chen::dns::edns0::opt> chen::dns::edns0::opt_chain::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::opt_chain::pack(dns::encoder &encoder) const
{
    encoder.pack(this->point, codec::StringType::Domain, false);
}

void chen::dns::edns0::opt_chain::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->point, codec::StringType::Domain);
}