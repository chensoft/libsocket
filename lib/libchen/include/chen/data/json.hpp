/**
 * A tiny json parser, support utf-8 only
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   http://www.json.org
 * @link   http://www.ietf.org/rfc/rfc4627.txt
 * -----------------------------------------------------------------------------
 * A JSON value can be an object, array, number, string, true, false, or null
 * We treat the number as double, like the javascript
 * -----------------------------------------------------------------------------
 * Usage:
 * >> // parse json string to object
 * >> auto dict = chen::json::parse("{\"author\": \"Jian Chen\", \"time\": 1458480325}");
 * >> auto json = dict.getObject();
 * >>
 * >> std::cout << "author: " << json.at("author").getString() << std::endl;
 * >> std::cout << "time: " << json.at("time").getInteger() << std::endl;
 *
 * >> // encode json object to string
 * >> std::cout << chen::json::stringify(dict, 2) << std::endl;
 */
#pragma once

#include <unordered_map>
#include <exception>
#include <cstdlib>
#include <cctype>
#include <locale>
#include <string>
#include <vector>
#include <cmath>
#include <chen/base/utf8.hpp>
#include <chen/base/str.hpp>
#include <chen/base/num.hpp>

namespace chen
{
    class json
    {
    public:
        typedef std::unordered_map<std::string, chen::json> object;
        typedef std::vector<chen::json> array;

        enum class Type {None, Object, Array, Number, String, True, False, Null};

        union Data
        {
            chen::json::object *o;
            chen::json::array *a;
            double d;
            std::string *s;
        };

        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : std::runtime_error(what) {}
        };
        
    public:
        /**
         * Constructor
         */
        json() = default;
        json(chen::json::Type type);

        json(const json &o);
        json(json &&o);

        json(const chen::json::object &v);
        json(chen::json::object &&v);

        json(const chen::json::array &v);
        json(chen::json::array &&v);

        json(double v);
        json(std::int32_t v);
        json(std::uint32_t v);

        json(const std::string &v);
        json(std::string &&v);
        json(const char *v);

        json(bool v);

        json(std::nullptr_t);

        virtual ~json();

        /**
         * Assignment
         */
        json& operator=(std::nullptr_t);

        json& operator=(const json &o);
        json& operator=(json &&o);

        json& operator=(const chen::json::object &v);
        json& operator=(chen::json::object &&v);

        json& operator=(const chen::json::array &v);
        json& operator=(chen::json::array &&v);

        json& operator=(double v);
        json& operator=(std::int32_t v);
        json& operator=(std::uint32_t v);

        json& operator=(const std::string &v);
        json& operator=(std::string &&v);
        json& operator=(const char *v);

        json& operator=(bool v);

    public:
        /**
         * Json parse, accept text, file or even iterators
         */
        static chen::json parse(const std::string &text, bool file = false);

        template <typename InputIterator>
        static chen::json parse(InputIterator cur, InputIterator end);

        /**
         * Json stringify
         */
        static std::string stringify(const chen::json &json, std::size_t space = 0);

    public:
        /**
         * Validate the json, throw exception if has error
         * according to the rfc4627, a json text must be a serialized object or array
         */
        static void validate(const std::string &text, bool file = false);

        template <typename InputIterator>
        static void validate(InputIterator cur, InputIterator end);

    public:
        /**
         * Get the json type
         */
        virtual chen::json::Type type() const;

        /**
         * Check the json type
         */
        virtual bool isObject() const;
        virtual bool isArray()  const;
        virtual bool isNumber() const;
        virtual bool isString() const;
        virtual bool isTrue()   const;
        virtual bool isFalse()  const;
        virtual bool isNull()   const;

        virtual bool isBool() const;
        virtual bool isNone() const;

    public:
        /**
         * Get value, throw exception if type is incorrect
         */
        virtual const chen::json::object& getObject() const;
        virtual const chen::json::array& getArray() const;
        virtual double getNumber() const;
        virtual std::int32_t getInteger() const;  // maybe losing precision
        virtual std::uint32_t getUnsigned() const;
        virtual const std::string& getString() const;
        virtual bool getBool() const;

        /**
         * Get value via operators
         * e.g:
         * >> chen::json json(115);
         * >> int val = json;
         */
        operator chen::json::object() const;
        operator chen::json::array() const;
        operator double() const;
        operator std::int32_t() const;
        operator std::uint32_t() const;
        operator std::string() const;
        operator bool() const;

        /**
         * Get value, user can modify its content
         * @caution the integer method does not exist because we store number as double
         * @caution the bool method does not exist because modify it will result in wrong type
         */
        virtual chen::json::object& getObject();
        virtual chen::json::array& getArray();
        virtual double& getNumber();
        virtual std::string& getString();

        /**
         * Convert value to the desired type as possible
         */
        virtual chen::json::object toObject() const;
        virtual chen::json::array toArray() const;
        virtual double toNumber() const;
        virtual std::int32_t toInteger() const;  // maybe losing precision
        virtual std::uint32_t toUnsigned() const;
        virtual std::string toString() const;
        virtual bool toBool() const;

        /**
         * Clear the internal state
         */
        virtual void clear();

    protected:
        /**
         * Throw syntax exception
         */
        template <typename InputIterator>
        static void exception(const InputIterator &beg, InputIterator &cur, InputIterator &end);

        /**
         * Advance to the next non-whitespace character
         */
        template <typename InputIterator>
        static bool advance(const InputIterator &beg, InputIterator &cur, InputIterator &end, bool require = true);

        /**
         * Decode specific type
         */
        template <typename InputIterator>
        static void decode(chen::json &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(chen::json::object &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(chen::json::array &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(double &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(std::string &out, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(bool v, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        template <typename InputIterator>
        static void decode(std::nullptr_t, const InputIterator &beg, InputIterator &cur, InputIterator &end);

        /**
         * Encode specific type
         */
        static void encode(const chen::json &v, std::size_t space, std::string &output, std::size_t &indent);

        static void encode(const chen::json::object &v,
                           std::size_t space,
                           std::string &output,
                           std::size_t &indent);
        static void encode(const chen::json::array &v,
                           std::size_t space,
                           std::string &output,
                           std::size_t &indent);
        static void encode(double v, std::string &output);
        static void encode(const std::string &v, std::string &output);
        static void encode(bool v, std::string &output);
        static void encode(std::nullptr_t, std::string &output);

    protected:
        Type _type = Type::None;
        Data _data = {nullptr};
    };
}

// parse
template <typename InputIterator>
chen::json chen::json::parse(InputIterator cur, InputIterator end)
{
    chen::json item;

    const InputIterator beg = cur;

    // trim left spaces
    if (!chen::json::advance(beg, cur, end, false))
        return item;

    // decode item
    chen::json::decode(item, beg, cur, end);

    // trim right spaces
    chen::json::advance(beg, cur, end, false);

    // should reach end
    if (cur != end)
        chen::json::exception(beg, cur, end);

    return item;
}

// validate
template <typename InputIterator>
void chen::json::validate(InputIterator cur, InputIterator end)
{
    const InputIterator beg = cur;

    if (!chen::json::advance(beg, cur, end, false))
        return;

    // a valid json must start by '{' or '['
    if ((*cur != '{') && (*cur != '['))
        chen::json::exception(beg, cur, end);

    // check syntax error
    chen::json::parse(beg, end);  // use beg, not cur, to report correct position
}

// exception
template <typename InputIterator>
void chen::json::exception(const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if (cur == end)
    {
        throw chen::json::error("json: unexpected end of input");
    }
    else
    {
        auto pos = chen::num::str(std::distance(beg, cur));
        auto tok = std::isprint(*cur) ? std::string(1, *cur) : chen::str::format("\\x%02x", static_cast<int>(*cur));

        throw chen::json::error(chen::str::format("json: unexpected token '%s' at position %s", tok.c_str(), pos.c_str()));
    }
}

// advance
template <typename InputIterator>
bool chen::json::advance(const InputIterator &beg, InputIterator &cur, InputIterator &end, bool require)
{
    // skip whitespaces
    while ((cur != end) && std::isspace(*cur))
        ++cur;

    // check if end
    if (cur == end)
    {
        if (require)
            chen::json::exception(beg, cur, end);
        else
            return false;
    }

    return true;
}

// decode type
template <typename InputIterator>
void chen::json::decode(chen::json &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    chen::json::advance(beg, cur, end);

    switch (*cur)
    {
        case '{':  // object
        {
            chen::json::object o;
            chen::json::decode(o, beg, cur, end);
            out = std::move(o);
        }
            break;

        case '[':  // array
        {
            chen::json::array a;
            chen::json::decode(a, beg, cur, end);
            out = std::move(a);
        }
            break;

        case '"':  // string
        {
            std::string s;
            chen::json::decode(s, beg, cur, end);
            out = std::move(s);
        }
            break;

        case 't':  // true
            chen::json::decode(true, beg, cur, end);
            out = true;
            break;

        case 'f':  // false
            chen::json::decode(false, beg, cur, end);
            out = false;
            break;

        case 'n':  // null
            chen::json::decode(nullptr, beg, cur, end);
            out = nullptr;
            break;

        default:
            if (std::isdigit(*cur) || (*cur == '-'))  // number
            {
                double d = 0;
                chen::json::decode(d, beg, cur, end);
                out = d;
            }
            else
            {
                chen::json::exception(beg, cur, end);
            }
            break;
    }
}

template <typename InputIterator>
void chen::json::decode(chen::json::object &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if ((cur == end) || (*cur != '{'))
        chen::json::exception(beg, cur, end);

    chen::json::advance(beg, ++cur, end);

    while (cur != end)
    {
        // skip spaces
        chen::json::advance(beg, cur, end);

        if (*cur == '}')
            break;

        // find key
        if (*cur != '"')
            chen::json::exception(beg, cur, end);

        std::string key;
        chen::json::decode(key, beg, cur, end);

        // find separator
        chen::json::advance(beg, cur, end);

        if (*cur != ':')  // separator
            chen::json::exception(beg, cur, end);

        chen::json::advance(beg, ++cur, end);

        // find value
        chen::json item;
        chen::json::decode(item, beg, cur, end);

        out[std::move(key)] = std::move(item);  // override if key already exist

        // find comma or ending
        chen::json::advance(beg, cur, end);

        if (*cur == ',')
        {
            chen::json::advance(beg, ++cur, end);

            // don't allow json like {"k":1,}
            if (*cur == '}')
                chen::json::exception(beg, cur, end);
        }
        else if (*cur == '}')
        {
            break;
        }
        else
        {
            chen::json::exception(beg, cur, end);
        }
    }

    if (cur == end)
        chen::json::exception(beg, cur, end);

    ++cur;
}

template <typename InputIterator>
void chen::json::decode(chen::json::array &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if ((cur == end) || (*cur != '['))
        chen::json::exception(beg, cur, end);

    chen::json::advance(beg, ++cur, end);

    while (cur != end)
    {
        if (*cur == ']')
            break;

        // find value
        chen::json item;
        chen::json::decode(item, beg, cur, end);

        out.emplace_back(std::move(item));

        // find comma or ending
        chen::json::advance(beg, cur, end);

        if (*cur == ',')
        {
            chen::json::advance(beg, ++cur, end);

            // don't allow json like [1,]
            if (*cur == ']')
                chen::json::exception(beg, cur, end);
        }
        else if (*cur == ']')
        {
            break;
        }
        else
        {
            chen::json::exception(beg, cur, end);
        }
    }

    if (cur == end)
        chen::json::exception(beg, cur, end);

    ++cur;
}

template <typename InputIterator>
void chen::json::decode(double &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if (cur == end)
        chen::json::exception(beg, cur, end);

    std::string str(1, *cur++);

    // must begin with '-' or '1' ~ '9' or like '0.123456'
    if (str[0] == '-')
    {
        if ((cur == end) || !std::isdigit(*cur))
        {
            chen::json::exception(beg, cur, end);
        }
        else if (*cur == '0')
        {
            str += *cur++;

            if ((cur != end) && std::isdigit(*cur))
                chen::json::exception(beg, cur, end);
        }
    }
    else if ((str[0] == '0') && std::isdigit(*cur))
    {
        // if the first char is '0', then the whole number must be 0
        chen::json::exception(beg, cur, end);
    }

    // collect integer parts
    while ((cur != end) && std::isdigit(*cur))
        str += *cur++;

    // collect decimal parts
    if ((cur != end) && (*cur == '.'))
    {
        str += *cur++;

        // first
        if ((cur != end) && std::isdigit(*cur))
            str += *cur++;
        else
            chen::json::exception(beg, cur, end);

        // digits
        while ((cur != end) && std::isdigit(*cur))
            str += *cur++;
    }

    // 'e' or 'E'
    if ((cur != end) && ((*cur == 'e') || (*cur == 'E')))
    {
        str += *cur++;

        // '+', '-' or digits
        if ((cur != end) && ((*cur == '+') || (*cur == '-') || std::isdigit(*cur)))
        {
            str += *cur++;

            // first is '+' or '-'
            if (!std::isdigit(str.back()))
            {
                if ((cur != end) && std::isdigit(*cur))
                    str += *cur++;
                else
                    chen::json::exception(beg, cur, end);
            }

            // last
            while ((cur != end) && std::isdigit(*cur))
                str += *cur++;
        }
        else
        {
            chen::json::exception(beg, cur, end);
        }
    }

    // check if number is overflow
    double d = std::atof(str.c_str());

    if (std::isinf(d))
    {
        auto pos = chen::num::str(std::distance(beg, cur) - str.size());
        throw chen::json::error(chen::str::format("json: number '%s' is overflow at position %s", str.c_str(), pos.c_str()));
    }
    else if (std::isnan(d))
    {
        auto pos = chen::num::str(std::distance(beg, cur) - str.size());
        throw chen::json::error(chen::str::format("json: number '%s' is invalid at position %s", str.c_str(), pos.c_str()));
    }

    out = d;
}

template <typename InputIterator>
void chen::json::decode(std::string &out, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    if ((cur == end) || (*cur != '"') || (++cur == end))
        chen::json::exception(beg, cur, end);

    char ch = *cur;

    while (ch != '"')
    {
        // control characters must use escape
        if ((ch >= 0) && (ch <= 31))  // see ASCII
        {
            auto pos = chen::num::str(std::distance(beg, cur));
            throw chen::json::error(chen::str::format("json: control character code '%d' is not escaped at position %s", static_cast<int>(ch), pos.c_str()));
        }

        // unescape characters
        if (ch == '\\')
        {
            if (++cur == end)
                chen::json::exception(beg, cur, end);
            else
                ch = *cur;

            switch (ch)
            {
                case '"':
                case '\\':
                case '/':
                    out += ch;
                    break;

                case 'b':
                    out += '\b';
                    break;

                case 'f':
                    out += '\f';
                    break;

                case 'n':
                    out += '\n';
                    break;

                case 'r':
                    out += '\r';
                    break;

                case 't':
                    out += '\t';
                    break;

                case 'u':
                {
                    // handle unicode character
                    char unicode[5];
                    unicode[4] = '\0';

                    for (auto i = 0; i < 4; ++i)
                    {
                        if (++cur == end)
                            chen::json::exception(beg, cur, end);

                        ch = *cur;

                        if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
                            unicode[i] = ch;  // char must in range of '0' ~ '9', 'a' ~ 'f', 'A' ~ 'F'
                        else
                            chen::json::exception(beg, cur, end);
                    }

                    // convert utf-16 to utf-8
                    try
                    {
                        out.append(chen::utf8::convert(static_cast<std::uint32_t>(std::strtol(unicode, nullptr, 16))));
                    }
                    catch (...)
                    {
                        // e.g: \uD83D\uDE00, it's a emoji character
                        auto pos = chen::num::str(std::distance(beg, cur) - 4);
                        throw chen::json::error(chen::str::format("json: invalid unicode char '\\u%s' at position %s", unicode, pos.c_str()));
                    }
                }
                    break;

                default:
                    chen::json::exception(beg, cur, end);
            }
        }
        else
        {
            out += ch;
        }

        if (++cur == end)
            chen::json::exception(beg, cur, end);
        else
            ch = *cur;
    }

    ++cur;
}

template <typename InputIterator>
void chen::json::decode(bool v, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    constexpr char t[] = "true";
    constexpr char f[] = "false";

    const char *s = v ? t : f;
    const auto  l = v ? sizeof(t) - 1 : sizeof(f) - 1;

    for (std::size_t i = 0; i < l; ++i, ++cur)
    {
        if ((cur == end) || (*cur != s[i]))
            chen::json::exception(beg, cur, end);
    }
}

template <typename InputIterator>
void chen::json::decode(std::nullptr_t, const InputIterator &beg, InputIterator &cur, InputIterator &end)
{
    constexpr char n[] = "null";
    constexpr auto len = sizeof(n) - 1;

    for (std::size_t i = 0; i < len; ++i, ++cur)
    {
        if ((cur == end) || (*cur != n[i]))
            chen::json::exception(beg, cur, end);
    }
}