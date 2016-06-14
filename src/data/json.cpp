/**
 * Created by Jian Chen
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/data/json.hpp>
#include <chen/base/num.hpp>
#include <chen/sys/sys.hpp>
#include <fstream>

using namespace chen;

// -----------------------------------------------------------------------------
// json
json::json(std::nullptr_t)
: _type(Type::Null)
{

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
    this->_data.o = new chen::json::object(v);
}

json::json(const chen::json::array &v)
: _type(Type::Array)
{
    this->_data.a = new chen::json::array(v);
}

json::json(chen::json::array &&v)
: _type(Type::Array)
{
    this->_data.a = new chen::json::array(v);
}

json::json(double v)
: _type(Type::Number)
{
    this->_data.d = v;
}

json::json(int v)
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
    this->_data.s = new std::string(v);
}

json::json(const char *v)
: _type(Type::String)
{
    this->_data.s = new std::string(v);
}

json::json(bool v)
: _type(v ? Type::True : Type::False)
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
        case Type::None:
            break;

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

        case Type::True:
        case Type::False:
            this->_data.b = o._data.b;
            break;

        case Type::Null:
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
    this->_data.o = new chen::json::object(v);

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
    this->_data.a = new chen::json::array(v);

    return *this;
}

json& json::operator=(double v)
{
    this->clear();

    this->_type   = Type::Number;
    this->_data.d = v;

    return *this;
}

json& json::operator=(int v)
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
    this->_data.s = new std::string(v);

    return *this;
}

json& json::operator=(const char *v)
{
    if ((this->_type == Type::String) && (this->_data.s->c_str() == v))
        return *this;

    this->clear();

    this->_type   = Type::String;
    this->_data.s = new std::string(v);

    return *this;
}

json& json::operator=(bool v)
{
    this->clear();

    this->_type   = v ? Type::True : Type::False;
    this->_data.b = v;

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
            stream.exceptions(std::ios::failbit | std::ios::badbit);
            stream.open(text.c_str(), std::ios_base::binary);

            std::istreambuf_iterator<char> cur(stream);
            return json::decode(cur, std::istreambuf_iterator<char>());
        }
        catch (const std::ios_base::failure &e)
        {
            throw error_general(str::format("json: decode %s: %s", text.c_str(), chen::sys::error().c_str()));
        }
    }
    else
    {
        return json::decode(text.begin(), text.end());
    }
}

std::string json::stringify(const chen::json &json, std::size_t space)
{
    std::string output;
    std::size_t indent = 0;
    json::encode(json, output, space, indent);
    return output;
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
        throw error_general("json: type is not object");
}

const chen::json::array& json::getArray() const
{
    if (this->_type == Type::Array)
        return *this->_data.a;
    else
        throw error_general("json: type is not array");
}

double json::getNumber() const
{
    if (this->_type == Type::Number)
        return this->_data.d;
    else
        throw error_general("json: type is not number");
}

int json::getInteger() const
{
    if (this->_type == Type::Number)
        return static_cast<int>(this->_data.d);
    else
        throw error_general("json: type is not number");
}

const std::string& json::getString() const
{
    if (this->_type == Type::String)
        return *this->_data.s;
    else
        throw error_general("json: type is not string");
}

bool json::getBool() const
{
    if ((this->_type == Type::True) || (this->_type == Type::False))
        return this->_data.b;
    else
        throw error_general("json: type is not bool");
}

// convert value
chen::json::object json::toObject() const
{
    static chen::json::object object;

    switch (this->_type)
    {
        case Type::None:
            return object;

        case Type::Object:
            return *this->_data.o;

        case Type::Array:
        case Type::Number:
        case Type::String:
        case Type::True:
        case Type::False:
        case Type::Null:
            return object;
    }
}

chen::json::array json::toArray() const
{
    static chen::json::array array;

    switch (this->_type)
    {
        case Type::None:
        case Type::Object:
            return array;

        case Type::Array:
            return *this->_data.a;

        case Type::Number:
        case Type::String:
        case Type::True:
        case Type::False:
        case Type::Null:
            return array;
    }
}

double json::toNumber() const
{
    switch (this->_type)
    {
        case Type::None:
        case Type::Object:
        case Type::Array:
            return 0.0;

        case Type::Number:
            return this->_data.d;

        case Type::String:
        {
            std::string &d = *this->_data.s;
            return std::atof(d.c_str());
        }

        case Type::True:
            return 1.0;

        case Type::False:
        case Type::Null:
            return 0.0;
    }
}

int json::toInteger() const
{
    switch (this->_type)
    {
        case Type::None:
        case Type::Object:
        case Type::Array:
            return 0;

        case Type::Number:
            return static_cast<int>(this->_data.d);

        case Type::String:
        {
            std::string &d = *this->_data.s;
            return std::atoi(d.c_str());
        }

        case Type::True:
            return 1;

        case Type::False:
        case Type::Null:
            return 0;
    }
}

std::string json::toString() const
{
    switch (this->_type)
    {
        case Type::None:
        case Type::Object:
        case Type::Array:
            return "";

        case Type::Number:
        {
            std::int64_t i = static_cast<std::int64_t>(this->_data.d);
            return (this->_data.d - i == 0) ? chen::num::str(i) : chen::num::str(this->_data.d);
        }

        case Type::String:
            return *this->_data.s;

        case Type::True:
            return "true";

        case Type::False:
            return "false";

        case Type::Null:
            return "null";
    }
}

bool json::toBool() const
{
    switch (this->_type)
    {
        case Type::None:
            return false;

        // treat object as true, like javascript
        case Type::Object:
        case Type::Array:
            return true;

        case Type::Number:
            return this->_data.d != 0.0;

        case Type::String:
            return !this->_data.s->empty();

        case Type::True:
            return true;

        case Type::False:
        case Type::Null:
            return false;
    }
}

// clear
void json::clear()
{
    switch (this->_type)
    {
        case Type::None:
            return;

        case Type::Object:
            delete this->_data.o;
            break;

        case Type::Array:
            delete this->_data.a;
            break;

        case Type::Number:
            break;

        case Type::String:
            delete this->_data.s;
            break;

        case Type::True:
        case Type::False:
        case Type::Null:
            break;
    }

    this->_type = Type::None;
    this->_data = {nullptr};
}

// encode type
void json::encode(const chen::json &v, std::string &output, std::size_t space, std::size_t &indent)
{
    switch (v.type())
    {
        case Type::None:
            break;

        case Type::Object:
            return json::encode(v.getObject(), output, space, indent);

        case Type::Array:
            return json::encode(v.getArray(), output, space, indent);

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

void json::encode(const chen::json::object &v, std::string &output, std::size_t space, std::size_t &indent)
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

        json::encode(val, output, space, indent);
    }

    if (space && !v.empty())
    {
        indent -= space;
        output += '\n';
        output.append(indent, ' ');
    }

    output += '}';
}

void json::encode(const chen::json::array &v, std::string &output, std::size_t space, std::size_t &indent)
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

        json::encode(v[i], output, space, indent);
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

void json::encode(bool v, std::string &output)
{
    output.append(v ? "true" : "false");
}

void json::encode(std::nullptr_t, std::string &output)
{
    output.append("null");
}