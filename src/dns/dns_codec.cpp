/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_codec.hpp>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// codec
codec::~codec()
{

}

// fqdn
bool codec::isFqdn(const std::string &name)
{
    return name.empty() ? false : name.back() == '.';
}

std::string& codec::fqdn(std::string &name)
{
    if (!codec::isFqdn(name))
        name += '.';
    return name;
}

std::string codec::fqdn(const std::string &name)
{
    std::string temp(name);
    codec::fqdn(temp);
    return temp;
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
    out.emplace_back(val);
}

void encoder::pack(std::uint16_t val, std::vector<std::uint8_t> &out)
{
    for (int i = 8; i >= 0; i -= 8)
        out.emplace_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(std::uint32_t val, std::vector<std::uint8_t> &out)
{
    for (int i = 24; i >= 0; i -= 8)
        out.emplace_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(std::uint64_t val, std::vector<std::uint8_t> &out)
{
    for (int i = 56; i >= 0; i -= 8)
        out.emplace_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(chen::dns::RRType val, std::vector<std::uint8_t> &out)
{
    encoder::pack(static_cast<std::underlying_type<chen::dns::RRType>::type>(val), out);
}

void encoder::pack(chen::dns::RRClass val, std::vector<std::uint8_t> &out)
{
    encoder::pack(static_cast<std::underlying_type<chen::dns::RRClass>::type>(val), out);
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
            out.emplace_back(0);  // size for next label

            for (std::size_t i = 0, len = val.size(); i < len; ++i)
            {
                char c = val[i];

                if (c == '.')
                {
                    out[out.size() - length - 1] = static_cast<std::uint8_t>(length);
                    out.emplace_back(0);  // size for next label

                    length = 0;
                }
                else
                {
                    ++length;

                    if (length > SIZE_LIMIT_LABEL)
                        throw error_size(str::format("dns: codec pack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

                    out.emplace_back(static_cast<std::uint8_t>(c));
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

        out.emplace_back(static_cast<std::uint8_t>(val.size()));
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
void decoder::unpack(std::int8_t &val, const_iterator &cur, const_iterator &end)
{
    decoder::unpack(reinterpret_cast<std::uint8_t&>(val), cur, end);
}

void decoder::unpack(std::int16_t &val, const_iterator &cur, const_iterator &end)
{
    decoder::unpack(reinterpret_cast<std::uint16_t&>(val), cur, end);
}

void decoder::unpack(std::int32_t &val, const_iterator &cur, const_iterator &end)
{
    decoder::unpack(reinterpret_cast<std::uint32_t&>(val), cur, end);
}

void decoder::unpack(std::int64_t &val, const_iterator &cur, const_iterator &end)
{
    decoder::unpack(reinterpret_cast<std::uint64_t&>(val), cur, end);
}

void decoder::unpack(std::uint8_t &val, const_iterator &cur, const_iterator &end)
{
    if (end - cur < 1)
        throw error_size("dns: codec unpack size is not enough, require 1 bytes");

    val = *cur++;
}

void decoder::unpack(std::uint16_t &val, const_iterator &cur, const_iterator &end)
{
    if (end - cur < 2)
        throw error_size("dns: codec unpack size is not enough, require 2 bytes");

    val = 0;

    for (std::size_t i = 0, len = sizeof(val); i < len; ++i)
        val |= *cur++ << (len - i - 1) * 8;
}

void decoder::unpack(std::uint32_t &val, const_iterator &cur, const_iterator &end)
{
    if (end - cur < 4)
        throw error_size("dns: codec unpack size is not enough, require 4 bytes");

    val = 0;

    for (std::size_t i = 0, len = sizeof(val); i < len; ++i)
        val |= *cur++ << (len - i - 1) * 8;
}

void decoder::unpack(std::uint64_t &val, const_iterator &cur, const_iterator &end)
{
    if (end - cur < 8)
        throw error_size("dns: codec unpack size is not enough, require 8 bytes");

    val = 0;

    for (std::size_t i = 0, len = sizeof(val); i < len; ++i)
        val |= *cur++ << (len - i - 1) * 8;
}

void decoder::unpack(chen::dns::RRType &val, const_iterator &cur, const_iterator &end)
{
    decoder::unpack(reinterpret_cast<std::underlying_type<chen::dns::RRType>::type&>(val), cur, end);
}

void decoder::unpack(chen::dns::RRClass &val, const_iterator &cur, const_iterator &end)
{
    decoder::unpack(reinterpret_cast<std::underlying_type<chen::dns::RRClass>::type&>(val), cur, end);
}

void decoder::unpack(std::string &val, bool domain, cache_type &cache, const_iterator &beg, const_iterator &cur, const_iterator &end)
{
    if (end - cur < 1)
        throw error_size("dns: codec unpack string size is zero");

    // check compression
    // if first two bits are 11, then the following 14 bits are string's location
    if ((*cur & 0xFF) == 0xC0)
    {
        auto byte = *cur++ & 0x3F;
        if (!*cur)
            throw error_size("dns: codec unpack string size is not enough, require 1 bytes");

        auto pos = static_cast<std::size_t>((byte << 8) | *cur++);
        auto it  = cache.find(pos);

        if (it == cache.end())
            throw error_size("dns: codec unpack string compressed data not found in cache");

        val = it->second;
        return;
    }


    std::size_t pos = static_cast<std::size_t>(std::distance(beg, cur));

    // no compression
    if (domain)
    {
        // Note:
        // value contains multiple labels, each label is split by a dot
        // one byte label length + label characters + ... + one byte ending
        std::size_t origin = val.size();
        std::size_t length = 0;

        try
        {
            while (*cur)
            {
                length = static_cast<std::size_t>(*cur) + 1;

                if (end - cur < length)
                    throw error_size(str::format("dns: codec unpack domain size is not enough, require %d bytes", length));

                // check limit
                if (length > SIZE_LIMIT_LABEL)
                    throw error_size(str::format("dns: codec unpack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

                ++cur;

                for (std::size_t i = 1; i < length; ++i)
                    val += *cur++;

                val += ".";
            }

            ++cur;  // the padding zero
        }
        catch (...)
        {
            val.erase(origin, val.size() - origin);
            throw;
        }
    }
    else
    {
        // Note:
        // value is plain text
        // one byte length + characters
        auto length = static_cast<std::size_t>(*cur) + 1;

        if (end - cur < length)
            throw error_size(str::format("dns: codec unpack string size is not enough, require %d bytes", length));

        ++cur;

        for (std::size_t i = 1; i < length; ++i)
            val += *cur++;
    }

    // cache it
    cache[pos] = val;
}

void decoder::unpack(std::vector<std::uint8_t> &val, std::size_t need, const_iterator &cur, const_iterator &end)
{
    if (end - cur < need)
        throw error_size(str::format("dns: codec unpack vector size is not enough, require %d bytes", need));

    while (need--)
        val.emplace_back(*cur++);
}