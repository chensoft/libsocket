/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/prot/dns/dns_codec.hpp>

// -----------------------------------------------------------------------------
// codec

// fqdn
bool chen::dns::codec::isFqdn(const std::string &name)
{
    return name.empty() ? false : name.back() == '.';
}

std::string& chen::dns::codec::fqdn(std::string &name)
{
    if (!codec::isFqdn(name))
        name += '.';
    return name;
}

std::string chen::dns::codec::fqdn(const std::string &name)
{
    std::string temp(name);
    return codec::fqdn(temp);
}

// pqdn
bool chen::dns::codec::isPqdn(const std::string &name, const std::string &fqdn)
{
    return !codec::isFqdn(name) && codec::isFqdn(fqdn) && str::prefix(fqdn, name);
}


// -----------------------------------------------------------------------------
// encoder

// property
const std::vector<std::uint8_t>& chen::dns::encoder::data() const
{
    return this->_data;
}

std::vector<std::uint8_t> chen::dns::encoder::move()
{
    this->_cache.clear();
    return std::move(this->_data);
}

std::size_t chen::dns::encoder::size() const
{
    return this->_data.size();
}

const std::unordered_map<std::string, std::uint16_t>& chen::dns::encoder::cache() const
{
    return this->_cache;
}

// reset
void chen::dns::encoder::reset()
{
    this->_data.clear();
    this->_cache.clear();
}

// change
void chen::dns::encoder::change(std::size_t pos, std::uint8_t byte)
{
    this->_data[pos] = byte;
}

// pack
void chen::dns::encoder::pack(RRType val)
{
    encoder::pack(static_cast<std::underlying_type<RRType>::type>(val));
}

void chen::dns::encoder::pack(RRClass val)
{
    encoder::pack(static_cast<std::underlying_type<RRClass>::type>(val));
}

void chen::dns::encoder::pack(edns0::OptCode val)
{
    encoder::pack(static_cast<std::underlying_type<edns0::OptCode>::type>(val));
}

void chen::dns::encoder::pack(const std::string &val, StringType type, bool compress)
{
    // according to rfc1035, section 4.1.4
    // the compression scheme allows a domain name in a message to be represented as either:
    //   - a sequence of labels ending in a zero octet
    //   - a pointer
    //   - a sequence of labels ending with a pointer
    switch (type)
    {
        case StringType::Plain:
            this->plain(val);
            break;

        case StringType::Domain:
            this->domain(val, compress);  // only compress domain name
            break;
    }
}

void chen::dns::encoder::pack(const std::vector<std::uint8_t> &val, std::size_t need)
{
    if (val.size() < need)
        throw std::runtime_error(str::format("dns: codec pack vector size is not enough, require %d bytes", need));

    this->_data.insert(this->_data.end(), val.begin(), val.begin() + need);
}

// string
void chen::dns::encoder::plain(const std::string &val)
{
    // Note:
    // value is plain text
    // one byte length + characters
    if (val.size() > SIZE_LIMIT_STRING)
        throw std::runtime_error(str::format("dns: codec pack string must be %d octets or less", SIZE_LIMIT_STRING));

    this->_data.emplace_back(static_cast<std::uint8_t>(val.size()));
    this->_data.insert(this->_data.end(), val.begin(), val.end());
}

void chen::dns::encoder::domain(const std::string &val, bool compress)
{
    // check fqdn
    if (!codec::isFqdn(val))
        throw std::runtime_error("dns: codec pack domain is not fqdn");

    // check total length
    // caution: this limit isn't name's length, it's the bytes after encoded
    // example: www.chensoft.com. will encoded as [3, w, w, w, 8, c, h, e, n, s, o, f, t, 3, c, o, m, 0]
    // the encoded bytes can't exceed than SIZE_LIMIT_DOMAIN
    if (val.size() + 1 > SIZE_LIMIT_DOMAIN)
        throw std::runtime_error(str::format("dns: codec pack domain must be %d octets or less", SIZE_LIMIT_DOMAIN - 1));

    // try to compress
    if (compress && this->compress(val))
        return;

    // Note:
    // value contains multiple labels, each label is split by a dot
    // one byte label length + label characters + ... + one zero byte ending
    std::uint8_t  length = 0;
    std::uint16_t cursor = static_cast<std::uint16_t>(this->_data.size());

    this->_data.emplace_back(0);  // size for next label

    std::size_t size = val.size();
    if (size <= 1)
        return;

    for (std::size_t i = 0; i < size; ++i)
    {
        char c = val[i];

        if (c == '.')
        {
            this->_data[this->_data.size() - length - 1] = length;
            this->_data.emplace_back(0);  // size for next label

            length = 0;
        }
        else
        {
            ++length;

            if (length > SIZE_LIMIT_LABEL)
                throw std::runtime_error(str::format("dns: codec pack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

            this->_data.emplace_back(static_cast<std::uint8_t>(c));
        }
    }

    // add string to cache
    this->_cache[val] = cursor;
}

bool chen::dns::encoder::compress(const std::string &val)
{
    for (auto &pair : this->_cache)
    {
        if (str::suffix(pair.first, val))
        {
            auto size_a = pair.first.size();
            auto size_b = val.size();
            auto offset = size_a - size_b;

            // val must in the beginning of text, or the prev char is '.'
            if ((size_a != size_b) && (pair.first[offset - 1] != '.'))
                continue;

            // first two bits are 11
            std::uint16_t location = static_cast<std::uint16_t>((pair.second + offset) | 0xC000);
            this->_data.emplace_back(static_cast<uint8_t>(location >> 8 & 0xFF));
            this->_data.emplace_back(static_cast<uint8_t>(location & 0xFF));

            return true;
        }
    }

    return false;
}


// -----------------------------------------------------------------------------
// decoder
chen::dns::decoder::decoder(iterator beg, iterator end) : _beg(beg), _cur(beg), _end(end)
{
}

// property
const chen::dns::decoder::iterator& chen::dns::decoder::beg() const
{
    return this->_beg;
}

const chen::dns::decoder::iterator& chen::dns::decoder::cur() const
{
    return this->_cur;
}

const chen::dns::decoder::iterator& chen::dns::decoder::end() const
{
    return this->_end;
}

// reset
void chen::dns::decoder::reset()
{
    this->_cur = this->_beg;
}

// unpack
void chen::dns::decoder::unpack(RRType &val)
{
    decoder::unpack(reinterpret_cast<std::underlying_type<RRType>::type&>(val));
}

void chen::dns::decoder::unpack(RRClass &val)
{
    decoder::unpack(reinterpret_cast<std::underlying_type<RRClass>::type&>(val));
}

void chen::dns::decoder::unpack(edns0::OptCode &val)
{
    decoder::unpack(reinterpret_cast<std::underlying_type<edns0::OptCode>::type&>(val));
}

void chen::dns::decoder::unpack(std::string &val, StringType type)
{
    val.clear();

    if (std::distance(this->_cur, this->_end) < 1)
        throw std::runtime_error("dns: codec unpack string size is zero");

    switch (type)
    {
        case StringType::Plain:
            this->plain(val);
            break;

        case StringType::Domain:
            this->domain(val);
            break;
    }
}

void chen::dns::decoder::unpack(std::vector<std::uint8_t> &val, std::size_t need)
{
    if (static_cast<std::size_t>(std::distance(this->_cur, this->_end)) < need)
        throw std::runtime_error(str::format("dns: codec unpack vector size is not enough, require %d bytes", need));

    while (need--)
        val.emplace_back(*this->_cur++);
}

void chen::dns::decoder::plain(std::string &val)
{
    // Note:
    // value is plain text
    // one byte length + characters
    auto length = static_cast<std::size_t>(*this->_cur) + 1;

    if (static_cast<std::size_t>(std::distance(this->_cur, this->_end)) < length)
        throw std::runtime_error(str::format("dns: codec unpack string size is not enough, require %d bytes", length));

    ++this->_cur;

    for (std::size_t i = 1; i < length; ++i)
        val += *this->_cur++;
}

void chen::dns::decoder::domain(std::string &val)
{
    // Note:
    // value contains multiple labels, each label is split by a dot
    // one byte label length + label characters + ... + one byte ending
    // if first two bits are 11, then the following 14 bits is string's location
    this->extract(val, this->_cur);
}

void chen::dns::decoder::extract(std::string &val, iterator &cur)
{
    do
    {
        // check compression
        if ((*cur & 0xC0) == 0xC0)
        {
            auto byte = *cur++ & 0x3F;
            if (!*cur)
                throw std::runtime_error("dns: codec unpack string size is not enough, require 1 bytes");

            auto pos = static_cast<std::size_t>((byte << 8) | *cur++);
            auto tmp = this->_beg;

            std::advance(tmp, pos);

            // recursive process
            return this->extract(val, tmp);
        }
        else if ((*cur & 0xC0) != 0)
        {
            // only bits 00 and 11 are defined in rfc1035
            // bits 01 and 10 are defined in rfc2671 and rfc2673
            // but they are not widely deployed and deprecated now
            throw std::runtime_error("dns: codec unpack string label type error");
        }

        // normal text
        auto length = *cur + 1;

        if (std::distance(cur, this->_end) < length)
            throw std::runtime_error(str::format("dns: codec unpack domain size is not enough, require %d bytes", length));

        // check limit
        if (static_cast<std::size_t>(length) > SIZE_LIMIT_LABEL)
            throw std::runtime_error(str::format("dns: codec unpack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

        for (auto i = 1; i < length; ++i)
            val += *++cur;

        val += '.';
    } while (*cur && *++cur);

    ++cur;  // skip the ending zero
}