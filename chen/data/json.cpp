/**
 * Created by Jian Chen
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "json.hpp"
#include <fstream>

using namespace chen;

// -----------------------------------------------------------------------------
// json
json::json(std::nullptr_t)
: _type(JsonType::Null)
{

}

json::json(const json &o)
{
    *this = o;
}

json::json(json &&o)
: _type(o._type)
, _data(o._data)
{
    o._type = JsonType::None;
    o._data = {nullptr};
}

json::json(const chen::json::object &v)
: _type(JsonType::Object)
{
    this->_data.o = new chen::json::object(v);
}

json::json(chen::json::object &&v)
: _type(JsonType::Object)
{
    this->_data.o = new chen::json::object(v);
}

json::json(const chen::json::array &v)
: _type(JsonType::Array)
{
    this->_data.a = new chen::json::array(v);
}

json::json(chen::json::array &&v)
: _type(JsonType::Array)
{
    this->_data.a = new chen::json::array(v);
}

json::json(double v)
: _type(JsonType::Number)
{
    this->_data.d = v;
}

json::json(int v)
: _type(JsonType::Number)
{
    this->_data.d = v;
}

json::json(const std::string &v)
: _type(JsonType::String)
{
    this->_data.s = new std::string(v);
}

json::json(std::string &&v)
: _type(JsonType::String)
{
    this->_data.s = new std::string(v);
}

json::json(const char *v)
: _type(JsonType::String)
{
    this->_data.s = new std::string(v);
}

json::json(bool v)
: _type(v ? JsonType::True : JsonType::False)
{
    this->_data.b = v;
}

json::~json()
{
    this->clear();
}

// assignment
json& json::operator=(std::nullptr_t)
{
    this->clear();

    this->_type = JsonType::Null;

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
        case JsonType::None:
            break;

        case JsonType::Object:
            this->_data.o = new chen::json::object(*o._data.o);
            break;

        case JsonType::Array:
            this->_data.a = new chen::json::array(*o._data.a);
            break;

        case JsonType::Number:
            this->_data.d = o._data.d;
            break;

        case JsonType::String:
            this->_data.s = new std::string(*o._data.s);
            break;

        case JsonType::True:
        case JsonType::False:
            this->_data.b = o._data.b;
            break;

        case JsonType::Null:
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

    o._type = JsonType::None;
    o._data = {nullptr};

    return *this;
}

json& json::operator=(const chen::json::object &v)
{
    if ((this->_type == JsonType::Object) && (this->_data.o == &v))
        return *this;

    this->clear();

    this->_type   = JsonType::Object;
    this->_data.o = new chen::json::object(v);

    return *this;
}

json& json::operator=(chen::json::object &&v)
{
    if ((this->_type == JsonType::Object) && (this->_data.o == &v))
        return *this;

    this->clear();

    this->_type   = JsonType::Object;
    this->_data.o = new chen::json::object(v);

    return *this;
}

json& json::operator=(const chen::json::array &v)
{
    if ((this->_type == JsonType::Array) && (this->_data.a == &v))
        return *this;

    this->clear();

    this->_type   = JsonType::Array;
    this->_data.a = new chen::json::array(v);

    return *this;
}

json& json::operator=(chen::json::array &&v)
{
    if ((this->_type == JsonType::Array) && (this->_data.a == &v))
        return *this;

    this->clear();

    this->_type   = JsonType::Array;
    this->_data.a = new chen::json::array(v);

    return *this;
}

json& json::operator=(double v)
{
    this->clear();

    this->_type   = JsonType::Number;
    this->_data.d = v;

    return *this;
}

json& json::operator=(int v)
{
    return *this = static_cast<double>(v);
}

json& json::operator=(const std::string &v)
{
    if ((this->_type == JsonType::String) && (this->_data.s == &v))
        return *this;

    this->clear();

    this->_type   = JsonType::String;
    this->_data.s = new std::string(v);

    return *this;
}

json& json::operator=(std::string &&v)
{
    if ((this->_type == JsonType::String) && (this->_data.s == &v))
        return *this;

    this->clear();

    this->_type   = JsonType::String;
    this->_data.s = new std::string(v);

    return *this;
}

json& json::operator=(const char *v)
{
    if ((this->_type == JsonType::String) && (this->_data.s->c_str() == v))
        return *this;

    this->clear();

    this->_type   = JsonType::String;
    this->_data.s = new std::string(v);

    return *this;
}

json& json::operator=(bool v)
{
    this->clear();

    this->_type   = v ? JsonType::True : JsonType::False;
    this->_data.b = v;

    return *this;
}

// parse & stringify
chen::json json::parse(const std::string &text, bool file)
{
    chen::json json;
    json.decode(text, file);
    return json;
}

std::string json::stringify(const chen::json &json, std::size_t space)
{
    return json.encode(space);
}

// decode
void json::decode(const std::string &text, bool file)
{
    if (file)
    {
        try
        {
            std::ifstream stream;
            stream.exceptions(std::ios::failbit | std::ios::badbit);
            stream.open(text.c_str(), std::ios_base::in | std::ios_base::binary);

            std::istreambuf_iterator<char> cur(stream);
            this->decode(cur, std::istreambuf_iterator<char>());
        }
        catch (const std::ios_base::failure &e)
        {
            throw error_general(errno ? ::strerror(errno) : "json decode error");
        }
    }
    else
    {
        this->decode(text.begin(), text.end());
    }
}

// encode
std::string json::encode(std::size_t space) const
{
    std::string output;
    std::size_t indent = 0;
    this->encode(output, space, indent);
    return output;
}

// clear
void json::clear()
{
    switch (this->_type)
    {
        case JsonType::None:
            return;

        case JsonType::Object:
            delete this->_data.o;
            break;

        case JsonType::Array:
            delete this->_data.a;
            break;

        case JsonType::Number:
            break;

        case JsonType::String:
            delete this->_data.s;
            break;

        case JsonType::True:
        case JsonType::False:
        case JsonType::Null:
            break;
    }

    this->_type = JsonType::None;
    this->_data = {nullptr};
}

// type
chen::json::JsonType json::type() const
{
    return this->_type;
}

// check
bool json::isObject() const
{
    return this->_type == JsonType::Object;
}

bool json::isArray() const
{
    return this->_type == JsonType::Array;
}

bool json::isNumber() const
{
    return this->_type == JsonType::Number;
}

bool json::isString() const
{
    return this->_type == JsonType::String;
}

bool json::isTrue() const
{
    return this->_type == JsonType::True;
}

bool json::isFalse() const
{
    return this->_type == JsonType::False;
}

bool json::isNull() const
{
    return this->_type == JsonType::Null;
}

bool json::isBool() const
{
    return (this->_type == JsonType::True) || (this->_type == JsonType::False);
}

bool json::isNone() const
{
    return this->_type == JsonType::None;
}

// get value
const chen::json::object& json::getObject() const
{
    if (this->_type == JsonType::Object)
        return *this->_data.o;
    else
        throw error_general("json type is not object");
}

const chen::json::array& json::getArray() const
{
    if (this->_type == JsonType::Array)
        return *this->_data.a;
    else
        throw error_general("json type is not array");
}

double json::getNumber() const
{
    if (this->_type == JsonType::Number)
        return this->_data.d;
    else
        throw error_general("json type is not number");
}

int json::getInteger() const
{
    if (this->_type == JsonType::Number)
        return static_cast<int>(this->_data.d);
    else
        throw error_general("json type is not number");
}

const std::string& json::getString() const
{
    if (this->_type == JsonType::String)
        return *this->_data.s;
    else
        throw error_general("json type is not string");
}

bool json::getBool() const
{
    if ((this->_type == JsonType::True) || (this->_type == JsonType::False))
        return this->_data.b;
    else
        throw error_general("json type is not bool");
}

// convert value
chen::json::object json::toObject() const
{
    static chen::json::object object;

    switch (this->_type)
    {
        case JsonType::None:
            return object;

        case JsonType::Object:
            return *this->_data.o;

        case JsonType::Array:
        case JsonType::Number:
        case JsonType::String:
        case JsonType::True:
        case JsonType::False:
        case JsonType::Null:
            return object;
    }
}

chen::json::array json::toArray() const
{
    static chen::json::array array;

    switch (this->_type)
    {
        case JsonType::None:
        case JsonType::Object:
            return array;

        case JsonType::Array:
            return *this->_data.a;

        case JsonType::Number:
        case JsonType::String:
        case JsonType::True:
        case JsonType::False:
        case JsonType::Null:
            return array;
    }
}

double json::toNumber() const
{
    switch (this->_type)
    {
        case JsonType::None:
        case JsonType::Object:
        case JsonType::Array:
            return 0.0;

        case JsonType::Number:
            return this->_data.d;

        case JsonType::String:
        {
            std::string &d = *this->_data.s;
            return std::atof(d.c_str());
        }

        case JsonType::True:
            return 1.0;

        case JsonType::False:
        case JsonType::Null:
            return 0.0;
    }
}

int json::toInteger() const
{
    switch (this->_type)
    {
        case JsonType::None:
        case JsonType::Object:
        case JsonType::Array:
            return 0;

        case JsonType::Number:
            return static_cast<int>(this->_data.d);

        case JsonType::String:
        {
            std::string &d = *this->_data.s;
            return std::atoi(d.c_str());
        }

        case JsonType::True:
            return 1;

        case JsonType::False:
        case JsonType::Null:
            return 0;
    }
}

std::string json::toString() const
{
    switch (this->_type)
    {
        case JsonType::None:
        case JsonType::Object:
        case JsonType::Array:
            return "";

        case JsonType::Number:
        {
            std::int64_t i = static_cast<std::int64_t>(this->_data.d);
            return (this->_data.d - i == 0) ? std::to_string(i) : std::to_string(this->_data.d);
        }

        case JsonType::String:
            return *this->_data.s;

        case JsonType::True:
            return "true";

        case JsonType::False:
            return "false";

        case JsonType::Null:
            return "null";
    }
}

bool json::toBool() const
{
    switch (this->_type)
    {
        case JsonType::None:
            return false;

        // treat object as true, like javascript
        case JsonType::Object:
        case JsonType::Array:
            return true;

        case JsonType::Number:
            return this->_data.d != 0.0;

        case JsonType::String:
            return !this->_data.s->empty();

        case JsonType::True:
            return true;

        case JsonType::False:
        case JsonType::Null:
            return false;
    }
}

// encode type
void json::encode(std::string &output, std::size_t space, std::size_t &indent) const
{
    switch (this->_type)
    {
        case JsonType::None:
            break;

        case JsonType::Object:
            return this->encode(this->getObject(), output, space, indent);

        case JsonType::Array:
            return this->encode(this->getArray(), output, space, indent);

        case JsonType::Number:
            return this->encode(this->getNumber(), output);

        case JsonType::String:
            return this->encode(this->getString(), output);

        case JsonType::True:
        case JsonType::False:
            return this->encode(this->getBool(), output);

        case JsonType::Null:
            return this->encode(nullptr, output);
    }
}

void json::encode(const chen::json::object &v, std::string &output, std::size_t space, std::size_t &indent) const
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

        val.encode(output, space, indent);
    }

    if (space && !v.empty())
    {
        indent -= space;
        output += '\n';
        output.append(indent, ' ');
    }

    output += '}';
}

void json::encode(const chen::json::array &v, std::string &output, std::size_t space, std::size_t &indent) const
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

        v[i].encode(output, space, indent);
    }

    if (space && !v.empty())
    {
        indent -= space;
        output += '\n';
        output.append(indent, ' ');
    }

    output += ']';
}

void json::encode(double v, std::string &output) const
{
    output.append(std::to_string(v));
}

void json::encode(const std::string &v, std::string &output) const
{
    // escape the control characters only
    // json can accept unicode chars, so we don't escape unicode
    // see http://www.json.org
    output += '"';

    for (auto ch : v)
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

void json::encode(bool v, std::string &output) const
{
    output.append(v ? "true" : "false");
}

void json::encode(std::nullptr_t, std::string &output) const
{
    output.append("null");
}