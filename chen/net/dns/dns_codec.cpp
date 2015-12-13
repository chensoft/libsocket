/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_codec.h"
#include "dns_error.h"
#include <chen/tool/str.h>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// codec

// rr
void codec::pack(const RR &rr, std::vector<std::uint8_t> &store)
{
    auto size = store.size();

    try
    {
        // common
        this->pack(rr.name, true, store);
        this->pack(static_cast<std::uint16_t>(rr.rrtype), store);
        this->pack(static_cast<std::uint16_t>(rr.rrclass), store);
        this->pack(static_cast<std::uint32_t>(rr.ttl), store);

        // rdata
        auto rdata = rr.data();
        this->pack(static_cast<std::uint16_t>(rdata.size()), store);
        store.insert(store.end(), rdata.cbegin(), rdata.cend());
    }
    catch (...)
    {
        // restore
        store.erase(store.begin() + size, store.end());
        throw;
    }
}

void codec::unpack(RR &rr, const std::vector<std::uint8_t> &data)
{

}

// clear
void codec::clear()
{
    this->_map.clear();
}

// data
void codec::pack(std::int8_t value, std::vector<std::uint8_t> &store)
{
    codec::pack(static_cast<std::uint8_t>(value), store);
}

void codec::pack(std::int16_t value, std::vector<std::uint8_t> &store)
{
    codec::pack(static_cast<std::uint16_t>(value), store);
}

void codec::pack(std::int32_t value, std::vector<std::uint8_t> &store)
{
    codec::pack(static_cast<std::uint32_t>(value), store);
}

void codec::pack(std::uint8_t value, std::vector<std::uint8_t> &store)
{
    store.push_back(value);
}

void codec::pack(std::uint16_t value, std::vector<std::uint8_t> &store)
{
    store.push_back(static_cast<std::uint8_t>(static_cast<std::uint16_t>(value) >> 8));
    store.push_back(static_cast<std::uint8_t>(static_cast<std::uint16_t>(value) & 0xFF));
}

void codec::pack(std::uint32_t value, std::vector<std::uint8_t> &store)
{
    store.push_back(static_cast<std::uint8_t>(value >> 24 & 0xFF));
    store.push_back(static_cast<std::uint8_t>(value >> 16 & 0xFF));
    store.push_back(static_cast<std::uint8_t>(value >> 8 & 0xFF));
    store.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

void codec::pack(chen::dns::RRType value, std::vector<std::uint8_t> &store)
{
    codec::pack(static_cast<std::uint16_t>(value), store);
}

void codec::pack(chen::dns::RRClass value, std::vector<std::uint8_t> &store)
{
    codec::pack(static_cast<std::uint16_t>(value), store);
}

void codec::pack(const std::string &value, bool domain, std::vector<std::uint8_t> &store)
{
    if (domain)
    {
        // Note:
        // value contains multiple labels, each label is split by a dot
        // one byte label length + label characters + ... + one byte ending

        // check fqdn
        if (codec::isFqdn(value))
            throw error_fqdn("codec pack domain is not fqdn");

        // check total length
        // caution: this limit isn't name's length, it's the bytes after encoded
        // example: www.chensoft.com. will encoded as [3, w, w, w, 8, c, h, e, n, s, o, f, t, 3, c, o, m, 0]
        // the encoded bytes can't exceed than SIZE_LIMIT_DOMAIN
        if (value.size() + 1 > SIZE_LIMIT_DOMAIN)
            throw error_size(str::format("codec pack domain must be %d octets or less", SIZE_LIMIT_DOMAIN - 1));

        // generate binary
        std::size_t origin = store.size();
        std::size_t length = 0;

        try
        {
            store.push_back(0);  // size for next label

            for (std::uint8_t i = 0, len = static_cast<std::uint8_t>(value.size()); i < len; ++i)
            {
                char c = value[i];

                if (c == '.')
                {
                    store[store.size() - length - 1] = static_cast<std::uint8_t>(length);
                    store.push_back(0);  // size for next label

                    length = 0;
                }
                else
                {
                    ++length;

                    if (length > SIZE_LIMIT_LABEL)
                        throw error_size(str::format("codec pack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

                    store.push_back(static_cast<std::uint8_t>(c));
                }
            }
        }
        catch (...)
        {
            store.erase(store.begin() + origin, store.end());
            throw;
        }
    }
    else
    {
        // Note:
        // value is plain text
        // one byte length + characters
        if (value.size() > SIZE_LIMIT_STRING)
            throw error_size(str::format("codec pack string must be %d octets or less", SIZE_LIMIT_STRING));

        store.push_back(static_cast<std::uint8_t>(value.size()));
        store.insert(store.end(), value.cbegin(), value.cend());
    }
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