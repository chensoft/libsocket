/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_codec.h"
#include "dns_error.h"
#include "dns_table.h"
#include <chen/tool/str.h>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// codec

// rr
void codec::pack(const chen::dns::RR &rr, std::vector<std::uint8_t> &store)
{
    auto size = store.size();

    try
    {
        // common
        codec::pack(rr.name, true, store);
        codec::pack(static_cast<std::uint16_t>(rr.rrtype), store);
        codec::pack(static_cast<std::uint16_t>(rr.rrclass), store);
        codec::pack(static_cast<std::uint32_t>(rr.ttl), store);

        // rdata
        auto rdata = rr.data();
        codec::pack(static_cast<std::uint16_t>(rdata.size()), store);
        store.insert(store.end(), rdata.cbegin(), rdata.cend());
    }
    catch (...)
    {
        // restore
        store.erase(store.begin() + size, store.end());
        throw;
    }
}

void codec::unpack(std::shared_ptr<chen::dns::RR> &rr, const std::vector<std::uint8_t> &data)
{
    auto begin  = data.data();
    auto remain = data.size();

    // name
    std::string name;
    std::size_t size = codec::unpack(name, true, begin, remain);

    begin  += size;
    remain -= size;

    // rrtype
    chen::dns::RRType rrtype = chen::dns::RRType::None;
    size = codec::unpack(rrtype, begin, remain);

    begin  += size;
    remain -= size;

    // rrclass
    chen::dns::RRClass rrclass = chen::dns::RRClass::None;
    size = codec::unpack(rrclass, begin, remain);

    begin  += size;
    remain -= size;

    // ttl
    std::int32_t ttl = 0;
    size = codec::unpack(ttl, begin, remain);

    begin  += size;
    remain -= size;

    // rdlength
    std::uint16_t rdlength = 0;
    size = codec::unpack(rdlength, begin, remain);

    begin  += size;
    remain -= size;

    // build
    std::shared_ptr<chen::dns::RR> record;
    auto build = table::rrTypeToBuild(rrtype);

    if (build)
        record = build();
    else
        record.reset(new chen::dns::Unknown);

    // rdata
    record->setData(begin, remain);

    // assign
    rr.swap(record);
}

// header
void codec::pack(const chen::dns::header &header, std::vector<std::uint8_t> &store)
{
    auto size = store.size();

    try
    {
        // id
        codec::pack(header.id(), store);

        // flag
        codec::pack(header.flag(), store);

        // question
        codec::pack(header.qdcount(), store);

        // answer
        codec::pack(header.ancount(), store);

        // authority
        codec::pack(header.nscount(), store);

        // additional
        codec::pack(header.arcount(), store);
    }
    catch (...)
    {
        // restore
        store.erase(store.begin() + size, store.end());
        throw;
    }
}

void codec::unpack(chen::dns::header &header, const std::vector<std::uint8_t> &data)
{
    auto begin  = data.data();
    auto remain = data.size();

    // id
    std::int16_t id = 0;
    std::size_t size = codec::unpack(id, begin, remain);

    begin  += size;
    remain -= size;

    // flag
    std::uint16_t flag = 0;
    size = codec::unpack(flag, begin, remain);

    begin  += size;
    remain -= size;

    // question
    std::uint16_t qdcount = 0;
    size = codec::unpack(qdcount, begin, remain);

    begin  += size;
    remain -= size;

    // answer
    std::uint16_t ancount = 0;
    size = codec::unpack(ancount, begin, remain);

    begin  += size;
    remain -= size;

    // authority
    std::uint16_t nscount = 0;
    size = codec::unpack(nscount, begin, remain);

    begin  += size;
    remain -= size;

    // additional
    std::uint16_t arcount = 0;
    codec::unpack(arcount, begin, remain);

    // set
    header.setId(id);
    header.setFlag(flag);
    header.setQdcount(qdcount);
    header.setAncount(ancount);
    header.setNscount(nscount);
    header.setArcount(arcount);
}

// data pack
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

// data unpack
std::size_t codec::unpack(std::int8_t &value, const std::uint8_t *data, std::size_t size)
{
    codec::check(1, size, "codec unpack size is not enough, require 1 byte");
    value = static_cast<std::int8_t>(data[0]);
    return 1;
}

std::size_t codec::unpack(std::int16_t &value, const std::uint8_t *data, std::size_t size)
{
    codec::check(2, size, "codec unpack size is not enough, require 2 bytes");
    value = static_cast<std::int16_t>((data[0] << 8) + data[1]);
    return 2;
}

std::size_t codec::unpack(std::int32_t &value, const std::uint8_t *data, std::size_t size)
{
    codec::check(4, size, "codec unpack size is not enough, require 4 bytes");
    value = static_cast<std::int32_t>((data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3]);
    return 4;
}

std::size_t codec::unpack(std::uint8_t &value, const std::uint8_t *data, std::size_t size)
{
    codec::check(1, size, "codec unpack size is not enough, require 1 byte");
    value = data[0];
    return 1;
}

std::size_t codec::unpack(std::uint16_t &value, const std::uint8_t *data, std::size_t size)
{
    codec::check(2, size, "codec unpack size is not enough, require 2 bytes");
    value = static_cast<std::uint16_t>((data[0] << 8) + data[1]);
    return 2;
}

std::size_t codec::unpack(std::uint32_t &value, const std::uint8_t *data, std::size_t size)
{
    codec::check(4, size, "codec unpack size is not enough, require 4 bytes");
    value = static_cast<std::uint32_t>((data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3]);
    return 4;
}

std::size_t codec::unpack(chen::dns::RRType &value, const std::uint8_t *data, std::size_t size)
{
    codec::check(2, size, "codec unpack size is not enough, require 2 bytes");
    value = static_cast<chen::dns::RRType>((data[0] << 8) + data[1]);
    return 2;
}

std::size_t codec::unpack(chen::dns::RRClass &value, const std::uint8_t *data, std::size_t size)
{
    codec::check(2, size, "codec unpack size is not enough, require 2 bytes");
    value = static_cast<chen::dns::RRClass>((data[0] << 8) + data[1]);
    return 2;
}

std::size_t codec::unpack(std::string &value, bool domain, const std::uint8_t *data, std::size_t size)
{
    codec::check(1, size, "codec unpack size is zero");

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
            while (*data)
            {
                length = static_cast<std::size_t>(data[0]) + 1;
                codec::check(length, size, str::format("codec unpack domain size is not enough, require %d bytes", length));

                // check limit
                if (length > SIZE_LIMIT_LABEL)
                    throw error_size(str::format("codec unpack domain label must be %d octets or less", SIZE_LIMIT_LABEL));

                for (std::size_t i = 0, len = length - 1; i < len; ++i)
                {
                    value += data[i + 1];
                }

                data  += length;
                value += ".";
            }
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
        total = static_cast<std::size_t>(data[0]) + 1;
        codec::check(total, size, str::format("codec unpack string size is not enough, require %d bytes", total));

        value.insert(value.size(), reinterpret_cast<const char*>(data), total);
    }

    return total;
}

// check
void codec::check(std::size_t require, std::size_t size, const std::string &msg)
{
    if (size < require)
        throw error_size(msg);
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