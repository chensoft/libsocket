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

using namespace chen;
using namespace chen::dns;
using namespace chen::dns::edns0;

// -----------------------------------------------------------------------------
// edns0 - Option
Option::Option(OptionCode code) : code(code)
{
}

void Option::encode(encoder &encoder) const
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
        throw error_codec("dns: codec pack option data size is overflow");

    auto tmp = static_cast<std::uint16_t>(off);

    encoder.change(len - tmp - 2, static_cast<std::uint8_t>(tmp >> 8 & 0xFF));
    encoder.change(len - tmp - 1, static_cast<std::uint8_t>(tmp & 0xFF));
}

void Option::decode(decoder &decoder)
{
    // unpack base fields
    decoder.unpack(this->code);
    decoder.unpack(this->length);

    // unpack subclass
    this->unpack(decoder);
}

std::shared_ptr<edns0::Option> Option::create(chen::dns::decoder &decoder)
{
    chen::dns::decoder detect(decoder);

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
LLQ::LLQ() : Option(OptionCode::LLQ)
{
}

std::string LLQ::str(const std::string &sep) const
{
    std::string ret("LLQ");

    ret += sep + chen::num::str(this->version);
    ret += sep + chen::num::str(this->llq_opcode);
    ret += sep + chen::num::str(this->error_code);
    ret += sep + chen::num::str(this->llq_id);
    ret += sep + chen::num::str(this->lease_life);

    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> LLQ::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void LLQ::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->version);
    encoder.pack(this->llq_opcode);
    encoder.pack(this->error_code);
    encoder.pack(this->llq_id);
    encoder.pack(this->lease_life);
}

void LLQ::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->version);
    decoder.unpack(this->llq_opcode);
    decoder.unpack(this->error_code);
    decoder.unpack(this->llq_id);
    decoder.unpack(this->lease_life);
}


// -----------------------------------------------------------------------------
// edns0 - UL
UL::UL() : Option(OptionCode::UL)
{
}

std::string UL::str(const std::string &sep) const
{
    std::string ret("UL");
    ret += sep + chen::num::str(this->lease);
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> UL::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void UL::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->lease);
}

void UL::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->lease);
}


// -----------------------------------------------------------------------------
// edns0 - NSID
NSID::NSID() : Option(OptionCode::NSID)
{
}

std::string NSID::str(const std::string &sep) const
{
    std::string ret("NSID");
    ret += sep + RR::escape(this->data);
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> NSID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NSID::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->data, this->data.size());
}

void NSID::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->data, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - DAU
DAU::DAU() : Option(OptionCode::DAU)
{
}

std::string DAU::str(const std::string &sep) const
{
    std::string ret("DAU");
    ret += sep + RR::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> DAU::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void DAU::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void DAU::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - DHU
DHU::DHU() : Option(OptionCode::DHU)
{
}

std::string DHU::str(const std::string &sep) const
{
    std::string ret("DHU");
    ret += sep + RR::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> DHU::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void DHU::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void DHU::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - N3U
N3U::N3U() : Option(OptionCode::N3U)
{
}

std::string N3U::str(const std::string &sep) const
{
    std::string ret("N3U");
    ret += sep + RR::escape(this->alg_code.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> N3U::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void N3U::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->alg_code, this->alg_code.size());
}

void N3U::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->alg_code, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - Subnet
Subnet::Subnet() : Option(OptionCode::Subnet)
{
}

std::string Subnet::str(const std::string &sep) const
{
    std::string ret("Client Subnet");

    ret += sep + this->address.str();
    ret += "/" + chen::num::str(this->source);
    ret += "/" + chen::num::str(this->scope);

    return ret;
}

std::shared_ptr<edns0::Option> Subnet::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void Subnet::pack(chen::dns::encoder &encoder) const
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
                throw error_codec("dns: codec pack edns0 subnet source prefix is greater than 32");

            if (!this->address.isIPv4())
                throw error_codec("dns: codec pack edns0 subnet address is not ipv4");

            auto bytes = this->address.network().bytes();

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                encoder.pack(bytes[i]);
        }
            break;

        case 2:
        {
            if (this->source > 128)
                throw error_codec("dns: codec pack edns0 subnet source prefix is greater than 128");

            if (!this->address.isIPv6())
                throw error_codec("dns: codec pack edns0 subnet address is not ipv6");

            auto bytes = this->address.network().bytes();

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                encoder.pack(bytes[i]);
        }
            break;

        default:
            throw error_codec("dns: codec pack edns0 subnet family not support");
    }
}

void Subnet::unpack(chen::dns::decoder &decoder)
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
                throw error_codec("dns: codec unpack edns0 subnet source prefix is greater than 32");

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
                throw error_codec("dns: codec unpack edns0 subnet source prefix is greater than 128");

            std::array<std::uint8_t, 16> addr{};

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                decoder.unpack(addr[15 - i]);

            this->address = ip::version6(addr, this->source);
        }
            break;

        default:
            throw error_codec("dns: codec unpack edns0 subnet family not support");
    }
}


// -----------------------------------------------------------------------------
// edns0 - EXPIRE
EXPIRE::EXPIRE() : Option(OptionCode::EXPIRE)
{
}

std::string EXPIRE::str(const std::string &sep) const
{
    std::string ret("EXPIRE");
    ret += sep + chen::num::str(this->expire);
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> EXPIRE::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void EXPIRE::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->expire);
}

void EXPIRE::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->expire);
}


// -----------------------------------------------------------------------------
// edns0 - COOKIE
COOKIE::COOKIE() : Option(OptionCode::COOKIE)
{
}

std::string COOKIE::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::edns0::Option> COOKIE::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void COOKIE::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->client_cookie, this->client_cookie.size());
    encoder.pack(this->server_cookie, this->server_cookie.size());
}

void COOKIE::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->client_cookie, 8);
    decoder.unpack(this->server_cookie, this->length - 8u);
}


// -----------------------------------------------------------------------------
// edns0 - Keepalive
Keepalive::Keepalive() : Option(OptionCode::Keepalive)
{
}

std::string Keepalive::str(const std::string &sep) const
{
    std::string ret("TCP Keepalive");
    ret += sep + chen::num::str(this->timeout);
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> Keepalive::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void Keepalive::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->timeout);
}

void Keepalive::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->timeout);
}


// -----------------------------------------------------------------------------
// edns0 - Padding
Padding::Padding() : Option(OptionCode::Padding)
{
}

std::string Padding::str(const std::string &sep) const
{
    std::string ret("Padding");
    ret += sep + RR::escape(this->padding.size());
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> Padding::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void Padding::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->padding, this->padding.size());
}

void Padding::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->padding, this->length);
}


// -----------------------------------------------------------------------------
// edns0 - CHAIN
CHAIN::CHAIN() : Option(OptionCode::CHAIN)
{
}

std::string CHAIN::str(const std::string &sep) const
{
    std::string ret("CHAIN");
    ret += sep + this->point;
    return ret;
}

std::shared_ptr<chen::dns::edns0::Option> CHAIN::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void CHAIN::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->point, codec::StringType::Domain, false);
}

void CHAIN::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->point, codec::StringType::Domain);
}