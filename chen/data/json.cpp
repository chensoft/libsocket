/**
 * Created by Jian Chen
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "json.hpp"
#include <cstdlib>
#include <codecvt>
#include <sstream>
#include <fstream>
#include <locale>
#include <cmath>
#include <chen/tool/num.hpp>
#include <chen/tool/str.hpp>

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
    try
    {
        if (file)
        {
            std::ifstream stream;
            stream.exceptions(std::ios::failbit | std::ios::badbit);
            stream.open(text.c_str(), std::ios_base::in | std::ios_base::binary);
            this->decode(stream);
        }
        else
        {
            std::istringstream stream(text);
            stream.exceptions(std::ios::failbit | std::ios::badbit);
            this->decode(stream);
        }
    }
    catch (const std::ios_base::failure &e)
    {
        throw error_general(std::strerror(errno));
    }
}

void json::decode(std::istream &stream)
{
    // trim left spaces
    this->advance(stream);

    // decode item
    chen::json item;
    this->decode(item, stream);

    // trim right spaces
    this->advance(stream, false);

    // should reach end
    if (stream && !stream.eof())
        throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);

    // assign item
    *this = std::move(item);
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
            return (this->_data.d - i == 0) ? num::str(i) : num::str(this->_data.d);
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

// advance
void json::advance(std::istream &stream, bool check)
{
    for (; stream && !stream.eof() && std::isspace(stream.peek()); stream.ignore())
        ;

    if (check && (!stream || stream.eof()))
        throw error_syntax("json unexpected end of input", stream);
}

// decode type
void json::decode(chen::json &out, std::istream &stream)
{
    if (!stream || stream.eof())
        throw error_syntax("json unexpected end of input", stream);

    switch (stream.peek())
    {
        case '{':  // object
        {
            chen::json::object o;
            this->decode(o, stream);
            out = std::move(o);
        }
            break;

        case '[':  // array
        {
            chen::json::array a;
            this->decode(a, stream);
            out = std::move(a);
        }
            break;

        case '"':  // string
        {
            std::string s;
            this->decode(s, stream);
            out = std::move(s);
        }
            break;

        case 't':  // true
            this->decode(true, stream);
            out = true;
            break;

        case 'f':  // false
            this->decode(false, stream);
            out = false;
            break;

        case 'n':  // null
            this->decode(nullptr, stream);
            out = nullptr;
            break;

        default:
            if (std::isdigit(stream.peek()) || (stream.peek() == '-'))  // number
            {
                double d = 0;
                this->decode(d, stream);
                out = d;
            }
            else
            {
                throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);
            }
            break;
    }
}

void json::decode(chen::json::object &out, std::istream &stream)
{
    if (!stream || stream.eof())
        throw error_syntax("json unexpected end of input", stream);
    else if (stream.peek() != '{')
        throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);

    stream.ignore();

    while (stream && !stream.eof())
    {
        this->advance(stream);

        if (stream.peek() == '}')
            break;

        // find key
        if (stream.peek() != '"')
            throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);

        std::string key;
        this->decode(key, stream);

        // find separator
        this->advance(stream);

        if (stream.peek() != ':')  // separator
            throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);

        stream.ignore();

        this->advance(stream);

        // find value
        chen::json item;
        this->decode(item, stream);

        out[std::move(key)] = std::move(item);

        // find comma or ending
        this->advance(stream);

        if (stream.peek() == ',')
            stream.ignore();
        else if (stream.peek() == '}')
            break;
        else
            throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);
    }

    // todo always false?
    if (!stream || stream.eof())
        throw error_syntax("json unexpected end of input", stream);

    stream.ignore();
}

void json::decode(chen::json::array &out, std::istream &stream)
{
    if (!stream || stream.eof())
        throw error_syntax("json unexpected end of input", stream);
    else if (stream.peek() != '[')
        throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);

    stream.ignore();

    while (stream && !stream.eof())
    {
        this->advance(stream);

        if (stream.peek() == ']')
            break;

        // find value
        chen::json item;
        this->decode(item, stream);

        out.push_back(std::move(item));

        // find comma or ending
        this->advance(stream);

        if (stream.peek() == ',')
            stream.ignore();
        else if (stream.peek() == ']')
            break;
        else
            throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);
    }

    // todo always false?
    if (!stream || stream.eof())
        throw error_syntax("json unexpected end of input", stream);

    stream.ignore();
}

void json::decode(double &out, std::istream &stream)
{
    // todo add two error methods, to throw end of input and token
    if (!stream || stream.eof())
        throw error_syntax("json unexpected end of input", stream);
    else if ((stream.peek() != '-') && !std::isdigit(stream.peek()))
        throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);

    std::string str(1, stream.get());

    // must begin with '-' or '1' ~ '9'
    if (str[0] == '-')
    {
        // todo add method to detect stream status and get next char
        stream.peek();

        if (!stream || stream.eof())
        {
            throw error_syntax("json unexpected end of input", stream);
        }
        else if (stream.peek() == '0')
        {
            str += stream.get();

            if (std::isdigit(stream.peek()))
            {
                stream.unget();
                throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);
            }
        }
    }
    else if ((str[0] == '0') && std::isdigit(stream.peek()))
    {
        // if the first char is '0', then the whole number must be 0
        throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);
    }

    // collect integer parts
    while (stream && !stream.eof() && std::isdigit(stream.peek()))
        str += stream.get();

    // collect decimal parts
    if (stream && !stream.eof() && (stream.peek() == '.'))
    {
        str += stream.get();

        // digits
        while (stream && !stream.eof() && std::isdigit(stream.peek()))
            str += stream.get();
    }

    // 'e' or 'E'
    if (stream && !stream.eof() && ((stream.peek() == 'e') || (stream.peek() == 'E')))
    {
        str += stream.get();

        // '+', '-' or digits
        if (stream && !stream.eof() && ((stream.peek() == '+') || (stream.peek() == '-') || std::isdigit(stream.peek())))
        {
            str += stream.get();

            while (stream && !stream.eof() && std::isdigit(stream.peek()))
                str += stream.get();
        }
    }

    // check if number is overflow
    double d = std::atof(str.c_str());

    if (std::isinf(d))
        throw error_syntax("json number is overflow: " + str, stream);
    else if (std::isnan(d))
        throw error_syntax("json number is incorrect: " + str, stream);

    out = d;
}

void json::decode(std::string &out, std::istream &stream)
{
    if (!stream || stream.eof())
        throw error_syntax("json unexpected end of input", stream);
    else if (stream.peek() != '"')  // begin quote
        throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);

    stream.ignore();

    while (stream && !stream.eof() && (stream.peek() != '"'))
    {
        // control characters must use escape
        if ((stream.peek() >= 0) && (stream.peek() <= 31))  // see ASCII
            throw error_syntax("json control character is not escaped", stream);

        // unescape characters
        if (stream.peek() == '\\')
        {
            stream.ignore();

            switch (stream.peek())
            {
                case '"':
                case '\\':
                case '/':
                    out += stream.get();
                    break;

                case 'b':
                    out += '\b';
                    stream.ignore();
                    break;

                case 'f':
                    out += '\f';
                    stream.ignore();
                    break;

                case 'n':
                    out += '\n';
                    stream.ignore();
                    break;

                case 'r':
                    out += '\r';
                    stream.ignore();
                    break;

                case 't':
                    out += '\t';
                    stream.ignore();
                    break;

                case 'u':
                {
                    stream.ignore();

                    // handle unicode character
                    char unicode[5] = {0};

                    for (auto i = 0; i < 4; ++i)
                    {
                        char ch = static_cast<char>(stream.get());

                        if (ch < 0)
                            throw error_syntax("json unexpected end of input", stream);
                        else if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
                            unicode[i] = ch;  // char must in range of '0' ~ '9', 'a' ~ 'f', 'A' ~ 'F'
                        else
                            throw error_syntax(str::format("json unexpected token '%c'", ch), stream);
                    }

                    // convert utf-16 to utf-8
                    try
                    {
                        std::u16string source(1, static_cast<char16_t>(std::strtol(unicode, nullptr, 16)));
                        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

                        out.append(convert.to_bytes(source));
                    }
                    catch (...)
                    {
                        // e.g: \uD83D\uDE00, it's a emoji character
                        throw error_syntax(str::format("json invalid unicode char \\u%s", unicode), stream);
                    }
                }
                    break;

                default:
                    throw error_syntax(str::format("json unexpected token '%c'", stream.peek()), stream);
            }
        }
        else
        {
            out += stream.get();
        }
    }

    if (!stream || stream.eof())
        throw error_syntax("json unexpected end of input", stream);

    stream.ignore();
}

void json::decode(bool v, std::istream &stream)
{
    static const std::string t("true");
    static const std::string f("false");

    auto &s = v ? t : f;

    for (std::size_t i = 0, l = s.size(); i < l; ++i)
    {
        char ch = static_cast<char>(stream.get());

        if (ch < 0)
            throw error_syntax("json unexpected end of input", stream);
        else if (ch != s[i])
            throw error_syntax(str::format("json unexpected token '%c'", ch), stream);
    }
}

void json::decode(std::nullptr_t, std::istream &stream)
{
    static const std::string n("null");

    for (std::size_t i = 0, l = n.size(); i < l; ++i)
    {
        char ch = static_cast<char>(stream.get());

        if (ch < 0)
            throw error_syntax("json unexpected end of input", stream);
        else if (ch != n[i])
            throw error_syntax(str::format("json unexpected token '%c'", ch), stream);
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
    output.append(num::str(v));
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
    output.append(num::str(v));
}

void json::encode(std::nullptr_t, std::string &output) const
{
    output.append("null");
}


// -----------------------------------------------------------------------------
// error_syntax
chen::json::error_syntax::error_syntax(const std::string &what, std::istream &stream)
: chen::json::error(what)
{
    // todo offset is -1 when error?
//    this->offset = stream.tellg();
}