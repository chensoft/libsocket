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
// edns0 - Subnet
Subnet::Subnet() : Option(OptionCode::Subnet)
{
}

std::string Subnet::str(const std::string &sep) const
{
    std::string ret("Client Subnet");

    ret += sep;
    ret += this->address->str();
    ret += "/" + chen::num::str(this->source);
    ret += "/" + chen::num::str(this->scope);

    return ret;
}

std::shared_ptr<edns0::Option> Subnet::clone() const
{
    return std::make_shared<Subnet>(*this);
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

            auto v4 = this->address->v4();
            if (!v4)
                throw error_codec("dns: codec pack edns0 subnet address is not ipv4");

            auto bytes = v4->network().bytes();

            for (int i = 0, len = static_cast<int>(std::ceil(this->source / 8)); i < len; ++i)
                encoder.pack(bytes[i]);
        }
            break;

        case 2:
        {
            if (this->source > 128)
                throw error_codec("dns: codec pack edns0 subnet source prefix is greater than 128");

            auto v6 = this->address->v6();
            if (!v6)
                throw error_codec("dns: codec pack edns0 subnet address is not ipv6");

            auto bytes = v6->network().bytes();

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

            this->address = std::make_shared<chen::ip::address_v4>(*reinterpret_cast<std::uint32_t*>(addr), this->source);
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

            this->address = std::make_shared<chen::ip::address_v6>(addr, this->source);
        }
            break;

        default:
            throw error_codec("dns: codec unpack edns0 subnet family not support");
    }
}