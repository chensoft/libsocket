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
    return codec::fqdn(temp);
}


// -----------------------------------------------------------------------------
// encoder

// property
const std::vector<std::uint8_t>& encoder::data() const
{
    return this->_data;
}

std::size_t encoder::size() const
{
    return this->_data.size();
}

// reset
void encoder::reset()
{
    this->_data.clear();
}

// change
void encoder::change(std::size_t pos, std::uint8_t byte)
{
    this->_data[pos] = byte;
}

// pack
void encoder::pack(std::int8_t val)
{
    encoder::pack(static_cast<std::uint8_t>(val));
}

void encoder::pack(std::int16_t val)
{
    encoder::pack(static_cast<std::uint16_t>(val));
}

void encoder::pack(std::int32_t val)
{
    encoder::pack(static_cast<std::uint32_t>(val));
}

void encoder::pack(std::int64_t val)
{
    encoder::pack(static_cast<std::uint64_t>(val));
}

void encoder::pack(std::uint8_t val)
{
    this->_data.emplace_back(val);
}

void encoder::pack(std::uint16_t val)
{
    for (int i = 8; i >= 0; i -= 8)
        this->_data.emplace_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(std::uint32_t val)
{
    for (int i = 24; i >= 0; i -= 8)
        this->_data.emplace_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(std::uint64_t val)
{
    for (int i = 56; i >= 0; i -= 8)
        this->_data.emplace_back(static_cast<std::uint8_t>(val >> i & 0xFF));
}

void encoder::pack(chen::dns::RRType val)
{
    encoder::pack(static_cast<std::underlying_type<chen::dns::RRType>::type>(val));
}

void encoder::pack(chen::dns::RRClass val)
{
    encoder::pack(static_cast<std::underlying_type<chen::dns::RRClass>::type>(val));
}

void encoder::pack(const std::string &val, bool domain)
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
        std::size_t length = 0;

        this->_data.emplace_back(0);  // size for next label

        for (std::size_t i = 0, len = val.size(); i < len; ++i)
        {
            char c = val[i];

            if (c == '.')
            {
                this->_data[this->_data.size() - length - 1] = static_cast<std::uint8_t>(length);
                this->_data.emplace_back(0);  // size for next label

                length = 0;
            }
            else
            {
                ++length;

                if (length > SIZE_LIMIT_LABEL)
                    throw error_size(str::format("dns: codec pack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

                this->_data.emplace_back(static_cast<std::uint8_t>(c));
            }
        }
    }
    else
    {
        // Note:
        // value is plain text
        // one byte length + characters
        if (val.size() > SIZE_LIMIT_STRING)
            throw error_size(str::format("dns: codec pack string must be %d octets or less", SIZE_LIMIT_STRING));

        this->_data.emplace_back(static_cast<std::uint8_t>(val.size()));
        this->_data.insert(this->_data.end(), val.begin(), val.end());
    }
}

void encoder::pack(const std::vector<std::uint8_t> &val, std::size_t need)
{
    if (val.size() < need)
        throw error_size(str::format("dns: codec pack vector size is not enough, require %d bytes", need));

    this->_data.insert(this->_data.end(), val.begin(), val.begin() + need);
}


// -----------------------------------------------------------------------------
// decoder
decoder::decoder(iterator beg, iterator end) : _beg(beg), _cur(beg), _end(end)
{
}

// property
const decoder::iterator& decoder::beg() const
{
    return this->_beg;
}

const decoder::iterator& decoder::cur() const
{
    return this->_cur;
}

const decoder::iterator& decoder::end() const
{
    return this->_end;
}

const std::map<std::size_t, std::string>& decoder::cache() const
{
    return this->_cache;
}

// reset
void decoder::reset()
{
    this->_cur = this->_beg;
}

// unpack
void decoder::unpack(std::int8_t &val)
{
    decoder::unpack(reinterpret_cast<std::uint8_t&>(val));
}

void decoder::unpack(std::int16_t &val)
{
    decoder::unpack(reinterpret_cast<std::uint16_t&>(val));
}

void decoder::unpack(std::int32_t &val)
{
    decoder::unpack(reinterpret_cast<std::uint32_t&>(val));
}

void decoder::unpack(std::int64_t &val)
{
    decoder::unpack(reinterpret_cast<std::uint64_t&>(val));
}

void decoder::unpack(std::uint8_t &val)
{
    if (std::distance(this->_cur, this->_end) < 1)
        throw error_size("dns: codec unpack size is not enough, require 1 bytes");

    val = *this->_cur++;
}

void decoder::unpack(std::uint16_t &val)
{
    if (std::distance(this->_cur, this->_end) < 2)
        throw error_size("dns: codec unpack size is not enough, require 2 bytes");

    val = 0;

    for (std::size_t i = 0, len = sizeof(val); i < len; ++i)
        val |= *this->_cur++ << (len - i - 1) * 8;
}

void decoder::unpack(std::uint32_t &val)
{
    if (std::distance(this->_cur, this->_end) < 4)
        throw error_size("dns: codec unpack size is not enough, require 4 bytes");

    val = 0;

    for (std::size_t i = 0, len = sizeof(val); i < len; ++i)
        val |= *this->_cur++ << (len - i - 1) * 8;
}

void decoder::unpack(std::uint64_t &val)
{
    if (std::distance(this->_cur, this->_end) < 8)
        throw error_size("dns: codec unpack size is not enough, require 8 bytes");

    val = 0;

    for (std::size_t i = 0, len = sizeof(val); i < len; ++i)
        val |= *this->_cur++ << (len - i - 1) * 8;
}

void decoder::unpack(chen::dns::RRType &val)
{
    decoder::unpack(reinterpret_cast<std::underlying_type<chen::dns::RRType>::type&>(val));
}

void decoder::unpack(chen::dns::RRClass &val)
{
    decoder::unpack(reinterpret_cast<std::underlying_type<chen::dns::RRClass>::type&>(val));
}

void decoder::unpack(std::string &val, bool domain)
{
    if (std::distance(this->_cur, this->_end) < 1)
        throw error_size("dns: codec unpack string size is zero");

    // check compression
    // if first two bits are 11, then the following 14 bits are string's location
    if ((*this->_cur & 0xFF) == 0xC0)
    {
        auto byte = *this->_cur++ & 0x3F;
        if (!*this->_cur)
            throw error_size("dns: codec unpack string size is not enough, require 1 bytes");

        auto pos = static_cast<std::size_t>((byte << 8) | *this->_cur++);
        auto it  = this->_cache.find(pos);

        if (it == this->_cache.end())
            throw error_size("dns: codec unpack string compressed data not found in cache");

        val = it->second;
        return;
    }

    std::size_t pos = static_cast<std::size_t>(std::distance(this->_beg, this->_cur));

    // no compression
    if (domain)
    {
        // Note:
        // value contains multiple labels, each label is split by a dot
        // one byte label length + label characters + ... + one byte ending
        std::size_t length = 0;

        while (*this->_cur)
        {
            length = static_cast<std::size_t>(*this->_cur) + 1;

            if (std::distance(this->_cur, this->_end) < length)
                throw error_size(str::format("dns: codec unpack domain size is not enough, require %d bytes", length));

            // check limit
            if (length > SIZE_LIMIT_LABEL)
                throw error_size(str::format("dns: codec unpack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

            ++this->_cur;

            for (std::size_t i = 1; i < length; ++i)
                val += *this->_cur++;

            val += ".";
        }

        ++this->_cur;  // the padding zero
    }
    else
    {
        // Note:
        // value is plain text
        // one byte length + characters
        auto length = static_cast<std::size_t>(*this->_cur) + 1;

        if (std::distance(this->_cur, this->_end) < length)
            throw error_size(str::format("dns: codec unpack string size is not enough, require %d bytes", length));

        ++this->_cur;

        for (std::size_t i = 1; i < length; ++i)
            val += *this->_cur++;
    }

    // cache it
    this->_cache[pos] = val;
}

void decoder::unpack(std::vector<std::uint8_t> &val, std::size_t need)
{
    if (std::distance(this->_cur, this->_end) < need)
        throw error_size(str::format("dns: codec unpack vector size is not enough, require %d bytes", need));

    while (need--)
        val.emplace_back(*this->_cur++);
}