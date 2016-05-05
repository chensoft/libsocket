/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "dns_codec.hpp"
#include "dns_table.hpp"
#include "dns_record.hpp"
#include "dns_packet.hpp"
#include <chen/tool/num.hpp>
#include <limits>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// codec
codec::~codec()
{

}

// binary
std::vector<std::uint8_t> codec::retrieve()
{
    // todo performance test
    return std::move(this->_binary);
}

const std::vector<std::uint8_t>& codec::binary() const
{
    return this->_binary;
}

// clear
void codec::clear()
{
    this->_binary.clear();
}

// fqdn
bool codec::isFqdn(const std::string &name)
{
    auto size = name.size();
    return size ? name[size - 1] == '.' : false;
}

std::string codec::fqdn(const std::string &name)
{
    return codec::isFqdn(name) ? name : name + ".";
}


// -----------------------------------------------------------------------------
// encoder

// pack
void encoder::pack(std::int8_t val, std::vector<std::uint8_t> &out)
{
    encoder::pack(static_cast<std::uint8_t>(val), out);
}

void encoder::pack(std::int16_t val, std::vector<std::uint8_t> &out)
{
    encoder::pack(static_cast<std::uint16_t>(val), out);
}

void encoder::pack(std::int32_t val, std::vector<std::uint8_t> &out)
{
    encoder::pack(static_cast<std::uint32_t>(val), out);
}

void encoder::pack(std::int64_t val, std::vector<std::uint8_t> &out)
{
    encoder::pack(static_cast<std::uint64_t>(val), out);
}

void encoder::pack(std::uint8_t val, std::vector<std::uint8_t> &out)
{
    out.push_back(val);
}

void encoder::pack(std::uint16_t val, std::vector<std::uint8_t> &out)
{
    for (int i = 8; i >= 0; i -= 8)
        out.push_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(std::uint32_t val, std::vector<std::uint8_t> &out)
{
    for (int i = 24; i >= 0; i -= 8)
        out.push_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(std::uint64_t val, std::vector<std::uint8_t> &out)
{
    for (int i = 56; i >= 0; i -= 8)
        out.push_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(chen::dns::RRType val, std::vector<std::uint8_t> &out)
{
    encoder::pack(static_cast<std::uint16_t>(val), out);
}

void encoder::pack(chen::dns::RRClass val, std::vector<std::uint8_t> &out)
{
    encoder::pack(static_cast<std::uint16_t>(val), out);
}

void encoder::pack(const std::string &val, bool domain, std::vector<std::uint8_t> &out)
{
    if (domain)
    {
        // Note:
        // value contains multiple labels, each label is split by a dot
        // one byte label length + label characters + ... + one byte ending

        // check fqdn
        if (!codec::isFqdn(val))
            throw error_fqdn("dns: codec pack domain is not fqdn");

        // check total length
        // caution: this limit isn't name's length, it's the bytes after encoded
        // example: www.chensoft.com. will encoded as [3, w, w, w, 8, c, h, e, n, s, o, f, t, 3, c, o, m, 0]
        // the encoded bytes can't exceed than SIZE_LIMIT_DOMAIN
        if (val.size() + 1 > SIZE_LIMIT_DOMAIN)
            throw error_size(str::format("dns: codec pack domain must be %d octets or less", SIZE_LIMIT_DOMAIN - 1));

        // generate binary
        std::size_t origin = out.size();
        std::size_t length = 0;

        try
        {
            out.push_back(0);  // size for next label

            for (std::size_t i = 0, len = val.size(); i < len; ++i)
            {
                char c = val[i];

                if (c == '.')
                {
                    out[out.size() - length - 1] = static_cast<std::uint8_t>(length);
                    out.push_back(0);  // size for next label

                    length = 0;
                }
                else
                {
                    ++length;

                    if (length > SIZE_LIMIT_LABEL)
                        throw error_size(str::format("dns: codec pack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

                    out.push_back(static_cast<std::uint8_t>(c));
                }
            }
        }
        catch (...)
        {
            out.erase(out.begin() + origin, out.end());
            throw;
        }
    }
    else
    {
        // Note:
        // value is plain text
        // one byte length + characters
        if (val.size() > SIZE_LIMIT_STRING)
            throw error_size(str::format("dns: codec pack string must be %d octets or less", SIZE_LIMIT_STRING));

        out.push_back(static_cast<std::uint8_t>(val.size()));
        out.insert(out.end(), val.begin(), val.end());
    }
}

void encoder::pack(const std::vector<std::uint8_t> &val, std::size_t need, std::vector<std::uint8_t> &out)
{
    if (val.size() < need)
        throw error_size(str::format("dns: codec pack vector size is not enough, require %d bytes", need));

    out.insert(out.end(), val.begin(), val.begin() + need);
}


// -----------------------------------------------------------------------------
// decoder

// unpack
void decoder::unpack(std::int8_t &value)
{
    auto temp = static_cast<std::uint8_t>(value);
    this->unpack(temp);

    value = static_cast<std::int8_t>(temp);
}

void decoder::unpack(std::int16_t &value)
{
    auto temp = static_cast<std::uint16_t>(value);
    this->unpack(temp);

    value = static_cast<std::int16_t>(temp);
}

void decoder::unpack(std::int32_t &value)
{
    auto temp = static_cast<std::uint32_t>(value);
    this->unpack(temp);

    value = static_cast<std::int32_t>(temp);
}

void decoder::unpack(std::int64_t &value)
{
    auto temp = static_cast<std::uint64_t>(value);
    this->unpack(temp);

    value = static_cast<std::int64_t>(temp);
}

void decoder::unpack(std::uint8_t &value)
{
    if (this->remain() < 1)
        throw error_size("dns: codec unpack size is not enough, require 1 bytes");

    value = this->_binary[this->_cursor];

    this->_cursor += 1;
}

void decoder::unpack(std::uint16_t &value)
{
    if (this->remain() < 2)
        throw error_size("dns: codec unpack size is not enough, require 2 bytes");

    auto ptr = reinterpret_cast<std::uint16_t*>(&this->_binary[this->_cursor]);
    value    = num::swap(*ptr);

    this->_cursor += 2;
}

void decoder::unpack(std::uint32_t &value)
{
    if (this->remain() < 4)
        throw error_size("dns: codec unpack size is not enough, require 4 bytes");

    auto ptr = reinterpret_cast<std::uint32_t*>(&this->_binary[this->_cursor]);
    value    = num::swap(*ptr);

    this->_cursor += 4;
}

void decoder::unpack(std::uint64_t &value)
{
    if (this->remain() < 8)
        throw error_size("dns: codec unpack size is not enough, require 8 bytes");

    auto ptr = reinterpret_cast<std::uint64_t*>(&this->_binary[this->_cursor]);
    value    = num::swap(*ptr);

    this->_cursor += 8;
}

void decoder::unpack(chen::dns::RRType &value)
{
    auto temp = static_cast<std::uint16_t>(value);
    this->unpack(temp);

    value = static_cast<chen::dns::RRType>(temp);
}

void decoder::unpack(chen::dns::RRClass &value)
{
    auto temp = static_cast<std::uint16_t>(value);
    this->unpack(temp);

    value = static_cast<chen::dns::RRClass>(temp);
}

void decoder::unpack(std::string &value, bool domain)
{
    if (this->remain() < 1)
        throw error_size("dns: codec unpack size is zero");

    std::size_t total = 0;

    if (domain)
    {
        // Note:
        // value contains multiple labels, each label is split by a dot
        // one byte label length + label characters + ... + one byte ending
        std::size_t origin = value.size();
        std::size_t length = 0;

        try
        {
            const std::uint8_t *data = &this->_binary[this->_cursor];
            std::size_t size = this->remain();

            while (*data)
            {
                length = static_cast<std::size_t>(data[0]) + 1;

                if (size < length)
                    throw error_size(str::format("dns: codec unpack domain size is not enough, require %d bytes", length));

                // check limit
                if (length > SIZE_LIMIT_LABEL)
                    throw error_size(str::format("dns: codec unpack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

                for (std::size_t i = 1; i < length; ++i)
                {
                    value += data[i];
                }

                data  += length;
                value += ".";
                total += length;
            }

            total += 1;  // the padding zero
        }
        catch (...)
        {
            value.erase(origin, value.size() - origin);
            throw;
        }
    }
    else
    {
        // Note:
        // value is plain text
        // one byte length + characters
        total = static_cast<std::size_t>(this->_binary[this->_cursor]) + 1;

        if (this->remain() < total)
            throw error_size(str::format("dns: codec unpack string size is not enough, require %d bytes", total));

        value.insert(value.size(), reinterpret_cast<const char*>(this->_binary[this->_cursor + 1]), total - 1);
    }

    this->_cursor += total;
}

void decoder::unpack(std::vector<std::uint8_t> &value, std::size_t need)
{
    if (this->remain() < need)
        throw error_size(str::format("dns: codec unpack vector size is not enough, require %d bytes", need));

    value.insert(value.end(), this->_binary.begin() + this->_cursor, this->_binary.begin() + this->_cursor + need);
    this->_cursor += need;
}

// assign
void decoder::assign(const std::vector<std::uint8_t> &value)
{
    this->_binary = value;
    this->reset();
}

// state
std::size_t decoder::cursor() const
{
    return this->_cursor;
}

std::size_t decoder::remain() const
{
    return this->_binary.size() - this->_cursor;
}

// reset & clear
void decoder::reset()
{
    this->_cursor = 0;
}

void decoder::clear()
{
    codec::clear();
    this->reset();
}