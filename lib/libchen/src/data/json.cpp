/**
 * Created by Jian Chen
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/data/json.hpp>
#include <chen/sys/sys.hpp>
#include <fstream>

using namespace chen;

// -----------------------------------------------------------------------------
// json
json::json(chen::json::Type type)
{
    // handy method to create default json
    switch (type)
    {
        case Type::None:
            break;

        case Type::Object:
            *this = chen::json::object();
            break;

        case Type::Array:
            *this = chen::json::array();
            break;

        case Type::Number:
            *this = 0;
            break;

        case Type::String:
            *this = "";
            break;

        case Type::True:
            *this = true;
            break;

        case Type::False:
            *this = false;
            break;

        default:
            *this = nullptr;
            break;
    }
}

json::json(const json &o)
{
    *this = o;
}

json::json(json &&o)
{
    *this = std::move(o);
}

json::json(const chen::json::object &v)
: _type(Type::Object)
{
    this->_data.o = new chen::json::object(v);
}

json::json(chen::json::object &&v)
: _type(Type::Object)
{
    this->_data.o = new chen::json::object(std::move(v));
}

json::json(const chen::json::array &v)
: _type(Type::Array)
{
    this->_data.a = new chen::json::array(v);
}

json::json(chen::json::array &&v)
: _type(Type::Array)
{
    this->_data.a = new chen::json::array(std::move(v));
}

json::json(double v)
: _type(Type::Number)
{
    this->_data.d = v;
}

json::json(std::int32_t v)
: _type(Type::Number)
{
    this->_data.d = v;
}

json::json(std::uint32_t v)
: _type(Type::Number)
{
    this->_data.d = v;
}

json::json(const std::string &v)
: _type(Type::String)
{
    this->_data.s = new std::string(v);
}

json::json(std::string &&v)
: _type(Type::String)
{
    this->_data.s = new std::string(std::move(v));
}

json::json(const char *v)
: _type(Type::String)
{
    this->_data.s = new std::string(v);
}

json::json(bool v)
: _type(v ? Type::True : Type::False)
{
}

json::json(std::nullptr_t)
: _type(Type::Null)
{

}

json::~json()
{
    this->clear();
}

// assignment
json& json::operator=(std::nullptr_t)
{
    this->clear();

    this->_type = Type::Null;

    return *this;
}

json& json::operator=(const json &o)
{
    if (this == &o)
        return *this;

    this->clear();

    this->_type = o._type;

    switch (this->_type)
    {
        case Type::Object:
            this->_data.o = new chen::json::object(*o._data.o);
            break;

        case Type::Array:
            this->_data.a = new chen::json::array(*o._data.a);
            break;

        case Type::Number:
            this->_data.d = o._data.d;
            break;

        case Type::String:
            this->_data.s = new std::string(*o._data.s);
            break;

        default:
            break;
    }

    return *this;
}

json& json::operator=(json &&o)
{
    if (this == &o)
        return *this;

    this->clear();

    this->_type = o._type;
    this->_data = o._data;

    o._type = Type::None;
    o._data = {nullptr};

    return *this;
}

json& json::operator=(const chen::json::object &v)
{
    if ((this->_type == Type::Object) && (this->_data.o == &v))
        return *this;

    this->clear();

    this->_type   = Type::Object;
    this->_data.o = new chen::json::object(v);

    return *this;
}

json& json::operator=(chen::json::object &&v)
{
    if ((this->_type == Type::Object) && (this->_data.o == &v))
        return *this;

    this->clear();

    this->_type   = Type::Object;
    this->_data.o = new chen::json::object(std::move(v));

    return *this;
}

json& json::operator=(const chen::json::array &v)
{
    if ((this->_type == Type::Array) && (this->_data.a == &v))
        return *this;

    this->clear();

    this->_type   = Type::Array;
    this->_data.a = new chen::json::array(v);

    return *this;
}

json& json::operator=(chen::json::array &&v)
{
    if ((this->_type == Type::Array) && (this->_data.a == &v))
        return *this;

    this->clear();

    this->_type   = Type::Array;
    this->_data.a = new chen::json::array(std::move(v));

    return *this;
}

json& json::operator=(double v)
{
    this->clear();

    this->_type   = Type::Number;
    this->_data.d = v;

    return *this;
}

json& json::operator=(int32_t v)
{
    return *this = static_cast<double>(v);
}

json& json::operator=(std::uint32_t v)
{
    return *this = static_cast<double>(v);
}

json& json::operator=(const std::string &v)
{
    if ((this->_type == Type::String) && (this->_data.s == &v))
        return *this;

    this->clear();

    this->_type   = Type::String;
    this->_data.s = new std::string(v);

    return *this;
}

json& json::operator=(std::string &&v)
{
    if ((this->_type == Type::String) && (this->_data.s == &v))
        return *this;

    this->clear();

    this->_type   = Type::String;
    this->_data.s = new std::string(std::move(v));

    return *this;
}

json& json::operator=(const char *v)
{
    return operator=(std::string(v));
}

json& json::operator=(bool v)
{
    this->clear();

    this->_type = v ? Type::True : Type::False;

    return *this;
}

// parse & stringify
chen::json json::parse(const std::string &text, bool file)
{
    if (file)
    {
        try
        {
            std::ifstream stream;
            stream.exceptions(std::ios::badbit | std::ios::failbit);
            stream.open(text.c_str(), std::ios_base::binary);

            std::istreambuf_iterator<char> cur(stream);
            return json::parse(cur, std::istreambuf_iterator<char>());
        }
        catch (const std::ios_base::failure&)
        {
            throw json::error(str::format("json: decode %s: %s", text.c_str(), chen::sys::error().c_str()));
        }
    }
    else
    {
        return json::parse(text.begin(), text.end());
    }
}

std::string json::stringify(const chen::json &json, std::size_t space)
{
    std::string output;
    std::size_t indent = 0;
    json::encode(json, space, output, indent);
    return output;
}

// validate
void json::validate(const std::string &text, bool file)
{
    if (file)
    {
        try
        {
            std::ifstream stream;
            stream.exceptions(std::ios::badbit | std::ios::failbit);
            stream.open(text.c_str(), std::ios_base::binary);

            std::istreambuf_iterator<char> cur(stream);
            json::validate(cur, std::istreambuf_iterator<char>());
        }
        catch (const std::ios_base::failure&)
        {
            throw json::error(str::format("json: decode %s: %s", text.c_str(), chen::sys::error().c_str()));
        }
    }
    else
    {
        json::validate(text.begin(), text.end());
    }
}

// type
chen::json::Type json::type() const
{
    return this->_type;
}

// check
bool json::isObject() const
{
    return this->_type == Type::Object;
}

bool json::isArray() const
{
    return this->_type == Type::Array;
}

bool json::isNumber() const
{
    return this->_type == Type::Number;
}

bool json::isString() const
{
    return this->_type == Type::String;
}

bool json::isTrue() const
{
    return this->_type == Type::True;
}

bool json::isFalse() const
{
    return this->_type == Type::False;
}

bool json::isNull() const
{
    return this->_type == Type::Null;
}

bool json::isBool() const
{
    return (this->_type == Type::True) || (this->_type == Type::False);
}

bool json::isNone() const
{
    return this->_type == Type::None;
}

// get value
const chen::json::object& json::getObject() const
{
    if (this->_type == Type::Object)
        return *this->_data.o;
    else
        throw json::error("json: type is not object");
}

const chen::json::array& json::getArray() const
{
    if (this->_type == Type::Array)
        return *this->_data.a;
    else
        throw json::error("json: type is not array");
}

double json::getNumber() const
{
    if (this->_type == Type::Number)
        return this->_data.d;
    else
        throw json::error("json: type is not number");
}

std::int32_t json::getInteger() const
{
    if (this->_type == Type::Number)
        return static_cast<std::int32_t>(this->_data.d);
    else
        throw json::error("json: type is not number");
}

std::uint32_t json::getUnsigned() const
{
    if (this->_type == Type::Number)
        return static_cast<std::uint32_t>(this->_data.d);
    else
        throw json::error("json: type is not number");
}

const std::string& json::getString() const
{
    if (this->_type == Type::String)
        return *this->_data.s;
    else
        throw json::error("json: type is not string");
}

bool json::getBool() const
{
    if (this->_type == Type::True)
        return true;
    else if (this->_type == Type::False)
        return false;
    else
        throw json::error("json: type is not bool");
}

// operator
json::operator chen::json::object() const
{
    return this->getObject();
}

json::operator chen::json::array() const
{
    return this->getArray();
}

json::operator double() const
{
    return this->getNumber();
}

json::operator std::int8_t() const
{
    return static_cast<std::int8_t>(this->getInteger());
}

json::operator std::uint8_t() const
{
    return static_cast<std::uint8_t>(this->getUnsigned());
}

json::operator std::int16_t() const
{
    return static_cast<std::int16_t>(this->getInteger());
}

json::operator std::uint16_t() const
{
    return static_cast<std::uint16_t>(this->getUnsigned());
}

json::operator std::int32_t() const
{
    return this->getInteger();
}

json::operator std::uint32_t() const
{
    return this->getUnsigned();
}

json::operator std::int64_t() const
{
    return static_cast<std::int64_t>(this->getNumber());
}

json::operator std::uint64_t() const
{
    return static_cast<std::uint64_t>(this->getNumber());
}

json::operator std::string() const
{
    return this->getString();
}

json::operator bool() const
{
    return this->getBool();
}

chen::json::object& json::getObject()
{
    if (this->_type == Type::Object)
        return *this->_data.o;
    else
        throw json::error("json: type is not object");
}

chen::json::array& json::getArray()
{
    if (this->_type == Type::Array)
        return *this->_data.a;
    else
        throw json::error("json: type is not array");
}

double& json::getNumber()
{
    if (this->_type == Type::Number)
        return this->_data.d;
    else
        throw json::error("json: type is not number");
}

std::string& json::getString()
{
    if (this->_type == Type::String)
        return *this->_data.s;
    else
        throw json::error("json: type is not string");
}

// convert value
chen::json::object json::toObject() const
{
    switch (this->_type)
    {
        case Type::Object:
            return *this->_data.o;

        default:
            return chen::json::object();
    }
}

chen::json::array json::toArray() const
{
    switch (this->_type)
    {
        case Type::Array:
            return *this->_data.a;

        default:
            return chen::json::array();
    }
}

double json::toNumber() const
{
    switch (this->_type)
    {
        case Type::Number:
            return this->_data.d;

        case Type::String:
        {
            std::string &d = *this->_data.s;
            return std::atof(d.c_str());
        }

        case Type::True:
            return 1.0;

        default:
            return 0.0;
    }
}

std::int32_t json::toInteger() const
{
    switch (this->_type)
    {
        case Type::Number:
            return static_cast<std::int32_t>(this->_data.d);

        case Type::String:
        {
            std::string &d = *this->_data.s;
            return std::atoi(d.c_str());
        }

        case Type::True:
            return 1;

        default:
            return 0;
    }
}

std::uint32_t json::toUnsigned() const
{
    switch (this->_type)
    {
        case Type::Number:
            return static_cast<std::uint32_t>(this->_data.d);

        case Type::String:
        {
            std::string &d = *this->_data.s;
            return static_cast<std::uint32_t>(std::strtoul(d.c_str(), nullptr, 10));
        }

        case Type::True:
            return 1;

        default:
            return 0;
    }
}

std::string json::toString() const
{
    switch (this->_type)
    {
        case Type::Number:
            return chen::num::str(this->_data.d);

        case Type::String:
            return *this->_data.s;

        case Type::True:
            return "true";

        case Type::False:
            return "false";

        case Type::Null:
            return "null";

        default:
            return "";
    }
}

bool json::toBool() const
{
    switch (this->_type)
    {
        // treat object as true, like javascript
        case Type::Object:
        case Type::Array:
            return true;

        case Type::Number:
            return !chen::num::equal(this->_data.d, 0.0);

        case Type::String:
            return !this->_data.s->empty();

        case Type::True:
            return true;

        default:
            return false;
    }
}

// clear
void json::clear()
{
    switch (this->_type)
    {
        case Type::Object:
            delete this->_data.o;
            break;

        case Type::Array:
            delete this->_data.a;
            break;

        case Type::String:
            delete this->_data.s;
            break;

        default:
            break;
    }

    this->_type = Type::None;
    this->_data = {nullptr};
}

// encode type
void json::encode(const chen::json &v, std::size_t space, std::string &output, std::size_t &indent)
{
    switch (v.type())
    {
        case Type::None:
            break;

        case Type::Object:
            return json::encode(v.getObject(), space, output, indent);

        case Type::Array:
            return json::encode(v.getArray(), space, output, indent);

        case Type::Number:
            return json::encode(v.getNumber(), output);

        case Type::String:
            return json::encode(v.getString(), output);

        case Type::True:
        case Type::False:
            return json::encode(v.getBool(), output);

        case Type::Null:
            return json::encode(nullptr, output);
    }
}

void json::encode(const chen::json::object &v, std::size_t space, std::string &output, std::size_t &indent)
{
    output += '{';

    if (space && !v.empty())
    {
        indent += space;
        output += '\n';
    }

    auto beg = v.begin();
    auto end = v.end();

    for (auto it = beg; it != end; ++it)
    {
        auto &key = it->first;
        auto &val = it->second;

        if (it != beg)
        {
            output += ',';

            if (space)
                output += '\n';
        }

        if (indent)
            output.append(indent, ' ');

        output += '"';
        output.append(key);
        output.append("\":");

        if (space)
            output += ' ';

        json::encode(val, space, output, indent);
    }

    if (space && !v.empty())
    {
        indent -= space;
        output += '\n';
        output.append(indent, ' ');
    }

    output += '}';
}

void json::encode(const chen::json::array &v, std::size_t space, std::string &output, std::size_t &indent)
{
    output += '[';

    if (space && !v.empty())
    {
        indent += space;
        output += '\n';
    }

    for (std::size_t i = 0, len = v.size(); i < len; ++i)
    {
        if (i)
        {
            output += ',';

            if (space)
                output += '\n';
        }

        if (indent)
            output.append(indent, ' ');

        json::encode(v[i], space, output, indent);
    }

    if (space && !v.empty())
    {
        indent -= space;
        output += '\n';
        output.append(indent, ' ');
    }

    output += ']';
}

void json::encode(double v, std::string &output)
{
    output.append(chen::num::str(v));
}

void json::encode(const std::string &v, std::string &output)
{
    // escape the control characters only
    // json can accept unicode chars, so we don't escape unicode
    // see http://www.json.org
    output += '"';

    for (auto &ch : v)
    {
        switch (ch)
        {
            case '"':
                output.append("\\\"");
                break;

            case '\\':
                output.append("\\\\");
                break;

            case '/':
                output.append("\\/");
                break;

            case '\b':
                output.append("\\b");
                break;

            case '\f':
                output.append("\\f");
                break;

            case '\n':
                output.append("\\n");
                break;

            case '\r':
                output.append("\\r");
                break;

            case '\t':
                output.append("\\t");
                break;

            default:
                output += ch;
                break;
        }
    }

    output += '"';
}

void json::encode(bool v, std::string &output)
{
    output.append(v ? "true" : "false");
}

void json::encode(std::nullptr_t, std::string &output)
{
    output.append("null");
}