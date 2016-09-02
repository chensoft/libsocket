/**
 * Created by Jian Chen
 * @since  2016.07.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_edns.hpp>
#include <socket/dns/dns_table.hpp>
#include <socket/dns/dns_codec.hpp>
#include <chen/base/num.hpp>
#include <cmath>

// -----------------------------------------------------------------------------
// edns0 - Option
chen::dns::edns0::Option::Option(OptionCode code) : code(code)
{
}

void chen::dns::edns0::Option::encode(dns::encoder &encoder) const
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

    if (off > std::numeric_limits<std::uint16_t>::max())
        throw codec_error("dns: codec pack option data size is overflow");

    auto tmp = static_cast<std::uint16_t>(off);

    encoder.change(len - tmp - 2, static_cast<std::uint8_t>(tmp >> 8 & 0xFF));
    encoder.change(len - tmp - 1, static_cast<std::uint8_t>(tmp & 0xFF));
}

void chen::dns::edns0::Option::decode(dns::decoder &decoder)
{
    // unpack base fields
    decoder.unpack(this->code);
    decoder.unpack(this->length);

    // unpack subclass
    this->unpack(decoder);
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::Option::create(dns::decoder &decoder)
{
    dns::decoder detect(decoder);

    // detect type
    OptionCode code = OptionCode::None;
    detect.unpack(code);

    // build opt
    std::shared_ptr<edns0::Option> opt = table::build(code);
    if (!opt)
        return nullptr;

    // decode it
    opt->decode(decoder);

    return opt;
}


// -----------------------------------------------------------------------------
// edns0 - LLQ
chen::dns::edns0::LLQ::LLQ() : Option(OptionCode::LLQ)
{
}

std::string chen::dns::edns0::LLQ::str(const std::string &sep) const
{
    std::string ret("LLQ");

    ret += sep + num::str(this->version);
    ret += sep + num::str(this->llq_opcode);
    ret += sep + num::str(this->error_code);
    ret += sep + num::str(this->llq_id);
    ret += sep + num::str(this->lease_life);

    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::LLQ::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::LLQ::pack(dns::encoder &encoder) const
{
    encoder.pack(this->version);
    encoder.pack(this->llq_opcode);
    encoder.pack(this->error_code);
    encoder.pack(this->llq_id);
    encoder.pack(this->lease_life);
}

void chen::dns::edns0::LLQ::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->version);
    decoder.unpack(this->llq_opcode);
    decoder.unpack(this->error_code);
    decoder.unpack(this->llq_id);
    decoder.unpack(this->lease_life);
}


// -----------------------------------------------------------------------------
// edns0 - UL
chen::dns::edns0::UL::UL() : Option(OptionCode::UL)
{
}

std::string chen::dns::edns0::UL::str(const std::string &sep) const
{
    std::string ret("UL");
    ret += sep + num::str(this->lease);
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::UL::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::UL::pack(dns::encoder &encoder) const
{
    encoder.pack(this->lease);
}

void chen::dns::edns0::UL::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->lease);
}


// -----------------------------------------------------------------------------
// edns0 - NSID
chen::dns::edns0::NSID::NSID() : Option(OptionCode::NSID)
{
}

std::string chen::dns::edns0::NSID::str(const std::string &sep) const
{
    std::string ret("NSID");
    ret += sep + RR::escape(this->data);
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::NSID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::NSID::pack(dns::encoder &encoder) const
{
    encoder.pack(this->data, this->data.size());
}

void chen::dns::edns0::NSID::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->data, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - DAU
chen::dns::edns0::DAU::DAU() : Option(OptionCode::DAU)
{
}

std::string chen::dns::edns0::DAU::str(const std::string &sep) const
{
    std::string ret("DAU");
    ret += sep + RR::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::DAU::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::DAU::pack(dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void chen::dns::edns0::DAU::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - DHU
chen::dns::edns0::DHU::DHU() : Option(OptionCode::DHU)
{
}

std::string chen::dns::edns0::DHU::str(const std::string &sep) const
{
    std::string ret("DHU");
    ret += sep + RR::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::DHU::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::DHU::pack(dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void chen::dns::edns0::DHU::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - N3U
chen::dns::edns0::N3U::N3U() : Option(OptionCode::N3U)
{
}

std::string chen::dns::edns0::N3U::str(const std::string &sep) const
{
    std::string ret("N3U");
    ret += sep + RR::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::N3U::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::N3U::pack(dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void chen::dns::edns0::N3U::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - Subnet
chen::dns::edns0::Subnet::Subnet() : Option(OptionCode::Subnet), address(chen::ip::address::Type::IPv4)
{
}

std::string chen::dns::edns0::Subnet::str(const std::string &sep) const
{
    std::string ret("Client Subnet");

    ret += sep + this->address.str();
    ret += "/" + num::str(this->source);
    ret += "/" + num::str(this->scope);

    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::Subnet::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::Subnet::pack(dns::encoder &encoder) const
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
                throw codec_error("dns: codec pack edns0 subnet source prefix is greater than 32");

            if (!this->address.isIPv4())
                throw codec_error("dns: codec pack edns0 subnet address is not ipv4");

            auto bytes = this->address.network().bytes();

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                encoder.pack(bytes[i]);
        }
            break;

        case 2:
        {
            if (this->source > 128)
                throw codec_error("dns: codec pack edns0 subnet source prefix is greater than 128");

            if (!this->address.isIPv6())
                throw codec_error("dns: codec pack edns0 subnet address is not ipv6");

            auto bytes = this->address.network().bytes();

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                encoder.pack(bytes[i]);
        }
            break;

        default:
            throw codec_error("dns: codec pack edns0 subnet family not support");
    }
}

void chen::dns::edns0::Subnet::unpack(dns::decoder &decoder)
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
                throw codec_error("dns: codec unpack edns0 subnet source prefix is greater than 32");

            std::uint8_t addr[4]{};

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                decoder.unpack(addr[3 - i]);

            this->address = ip::version4(*reinterpret_cast<std::uint32_t*>(addr), this->source);
        }
            break;

        case 2:
        {
            // IPv6
            if (this->source > 128)
                throw codec_error("dns: codec unpack edns0 subnet source prefix is greater than 128");

            std::uint8_t addr[16]{};

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                decoder.unpack(addr[15 - i]);

            this->address = ip::version6(addr, this->source);
        }
            break;

        default:
            throw codec_error("dns: codec unpack edns0 subnet family not support");
    }
}


// -----------------------------------------------------------------------------
// edns0 - EXPIRE
chen::dns::edns0::EXPIRE::EXPIRE() : Option(OptionCode::EXPIRE)
{
}

std::string chen::dns::edns0::EXPIRE::str(const std::string &sep) const
{
    std::string ret("EXPIRE");
    ret += sep + num::str(this->expire);
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::EXPIRE::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::EXPIRE::pack(dns::encoder &encoder) const
{
    encoder.pack(this->expire);
}

void chen::dns::edns0::EXPIRE::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->expire);
}


// -----------------------------------------------------------------------------
// edns0 - COOKIE
chen::dns::edns0::COOKIE::COOKIE() : Option(OptionCode::COOKIE)
{
}

std::string chen::dns::edns0::COOKIE::str(const std::string &sep) const
{
    std::string ret("COOKIE");

    if (this->server_cookie.empty())
    {
        ret += sep + RR::escape(this->client_cookie.size());
    }
    else
    {
        ret += sep + RR::escape(this->client_cookie.size());
        ret += sep + RR::escape(this->server_cookie.size());
    }

    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::COOKIE::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::COOKIE::pack(dns::encoder &encoder) const
{
    encoder.pack(this->client_cookie, this->client_cookie.size());
    encoder.pack(this->server_cookie, this->server_cookie.size());
}

void chen::dns::edns0::COOKIE::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->client_cookie, 8);
    decoder.unpack(this->server_cookie, this->length - 8u);
}


// -----------------------------------------------------------------------------
// edns0 - Keepalive
chen::dns::edns0::Keepalive::Keepalive() : Option(OptionCode::Keepalive)
{
}

std::string chen::dns::edns0::Keepalive::str(const std::string &sep) const
{
    std::string ret("TCP Keepalive");
    ret += sep + num::str(this->timeout);
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::Keepalive::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::Keepalive::pack(dns::encoder &encoder) const
{
    encoder.pack(this->timeout);
}

void chen::dns::edns0::Keepalive::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->timeout);
}


// -----------------------------------------------------------------------------
// edns0 - Padding
chen::dns::edns0::Padding::Padding() : Option(OptionCode::Padding)
{
}

std::string chen::dns::edns0::Padding::str(const std::string &sep) const
{
    std::string ret("Padding");
    ret += sep + RR::escape(this->padding.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::Padding::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::Padding::pack(dns::encoder &encoder) const
{
    encoder.pack(this->padding, this->padding.size());
}

void chen::dns::edns0::Padding::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->padding, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - CHAIN
chen::dns::edns0::CHAIN::CHAIN() : Option(OptionCode::CHAIN)
{
}

std::string chen::dns::edns0::CHAIN::str(const std::string &sep) const
{
    std::string ret("CHAIN");
    ret += sep + this->point;
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> chen::dns::edns0::CHAIN::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::edns0::CHAIN::pack(dns::encoder &encoder) const
{
    encoder.pack(this->point, codec::StringType::Domain, false);
}

void chen::dns::edns0::CHAIN::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->point, codec::StringType::Domain);
}