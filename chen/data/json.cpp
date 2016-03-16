/**
 * Created by Jian Chen
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "json.hpp"
#include <cstdlib>
#include <chen/tool/num.hpp>

using namespace chen;

// -----------------------------------------------------------------------------
// json
json::json(const json &o)
: _type(o._type)
, _data(o._data)
{

}

json::json(json &&o)
: _type(o._type)
, _data(o._data)
{

}

json::json(const chen::json::object &v)
: _type(JsonType::Object)
, _data(v)
{

}

json::json(chen::json::object &&v)
: _type(JsonType::Object)
, _data(chen::json::object(v))
{

}

json::json(const chen::json::array &v)
: _type(JsonType::Array)
, _data(v)
{

}

json::json(chen::json::array &&v)
: _type(JsonType::Array)
, _data(chen::json::array(v))
{

}

json::json(double v)
: _type(JsonType::Number)
, _data(v)
{

}

json::json(int v)
: _type(JsonType::Number)
, _data(static_cast<double>(v))
{

}

json::json(const std::string &v)
: _type(JsonType::String)
, _data(std::string(v))
{

}

json::json(std::string &&v)
: _type(JsonType::String)
, _data(std::string(v))
{

}

json::json(const char *v)
: _type(JsonType::String)
, _data(std::string(v))
{

}

json::json(bool v)
: _type(v ? JsonType::True : JsonType::False)
, _data(v)
{

}

// parse
void json::parse(const std::string &text)
{

}

// clear
void json::clear()
{
    this->_type = JsonType::Null;
    this->_data.clear();
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

// value
chen::json::object json::asObject() const
{
    static chen::json::object object;

    switch (this->_type)
    {
        case JsonType::Object:
            return this->_data;

        case JsonType::Array:
        case JsonType::Number:
        case JsonType::String:
        case JsonType::True:
        case JsonType::False:
        case JsonType::Null:
            return object;
    }
}

chen::json::array json::asArray() const
{
    static chen::json::array array;

    switch (this->_type)
    {
        case JsonType::Object:
            return array;

        case JsonType::Array:
            return this->_data;

        case JsonType::Number:
        case JsonType::String:
        case JsonType::True:
        case JsonType::False:
        case JsonType::Null:
            return array;
    }
}

double json::asNumber() const
{
    switch (this->_type)
    {
        case JsonType::Object:
        case JsonType::Array:
            return 0.0;

        case JsonType::Number:
            return static_cast<double>(this->_data);

        case JsonType::String:
        {
            std::string d = this->_data;
            return std::atof(d.c_str());
        }

        case JsonType::True:
            return 1.0;

        case JsonType::False:
        case JsonType::Null:
            return 0.0;
    }
}

std::string json::asString() const
{
    switch (this->_type)
    {
        case JsonType::Object:
        case JsonType::Array:
            return "";

        case JsonType::Number:
        {
            // todo check if prevent extra zeros
            double d = static_cast<double>(this->_data);
            std::int64_t i = static_cast<std::int64_t>(d);
            return (d - i == 0) ? num::str(i) : num::str(d);
        }

        case JsonType::String:
            return this->_data;

        case JsonType::True:
            return "true";

        case JsonType::False:
            return "false";

        case JsonType::Null:
            return "null";
    }
}

bool json::asBool() const
{
    switch (this->_type)
    {
        // treat object as true, like javascript
        case JsonType::Object:
        case JsonType::Array:
            return true;

        case JsonType::Number:
            return static_cast<double>(this->_data) != 0.0;

        case JsonType::String:
        {
            std::string d = this->_data;
            return !d.empty();
        }

        case JsonType::True:
            return true;

        case JsonType::False:
        case JsonType::Null:
            return false;
    }
}

// copy & move
json& json::operator=(const json &o)
{
    if (this == &o)
        return *this;

    this->clear();

    this->_type = o._type;
    this->_data = o._data;

    return *this;
}

json& json::operator=(json &&o)
{
    if (this == &o)
        return *this;

    this->clear();

    this->_type = o._type;
    this->_data = o._data;

    return *this;
}