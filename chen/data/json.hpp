/**
 * A tiny json parser, support utf-8 only
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   http://www.json.org
 * -----------------------------------------------------------------------------
 * A JSON value can be an object, array, number, string, true, false, or null
 * we represent the number as double type, like the javascript
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

#include <string.h>
#include <exception>
#include <cstdlib>
#include <codecvt>
#include <locale>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <chen/tool/str.hpp>

namespace chen
{
    class json
    {
    public:
        typedef std::map<std::string, chen::json> object;
        typedef std::vector<chen::json> array;

        enum class JsonType {None, Object, Array, Number, String, True, False, Null};

        union JsonData
        {
            chen::json::object *o;
            chen::json::array *a;
            double d;
            std::string *s;
            bool b;
        };

        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : std::runtime_error(what) {}
        };

        class error_general : public chen::json::error
        {
        public:
            explicit error_general(const std::string &what) : chen::json::error(what) {}
        };

        class error_syntax : public chen::json::error
        {
        public:
            // todo record line and column as chen::json data member
            explicit error_syntax(const std::string &what) : chen::json::error(what) {}
        };

    public:
        /**
         * Constructor
         */
        json() = default;
        json(std::nullptr_t);

        json(const json &o);
        json(json &&o);

        json(const chen::json::object &v);
        json(chen::json::object &&v);

        json(const chen::json::array &v);
        json(chen::json::array &&v);

        json(double v);
        json(int v);

        json(const std::string &v);
        json(std::string &&v);
        json(const char *v);

        json(bool v);

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
        json& operator=(int v);

        json& operator=(const std::string &v);
        json& operator=(std::string &&v);
        json& operator=(const char *v);

        json& operator=(bool v);

    public:
        /**
         * Json parse and stringify helper
         * use encode and decode internally
         */
        static chen::json parse(const std::string &text, bool file = false);
        static std::string stringify(const chen::json &json,
                                     std::size_t space = 0);

    public:
        /**
         * Decode the json text, throw exception if found error
         */
        virtual void decode(const std::string &text, bool file = false);

        template <class InputIterator>
        void decode(InputIterator cur, InputIterator end);

        /**
         * Encode the json object to a string
         * @param space the indent count when using pretty print
         */
        virtual std::string encode(std::size_t space = 0) const;

        /**
         * Clear the internal state
         */
        virtual void clear();

    public:
        /**
         * Get the json type
         */
        virtual chen::json::JsonType type() const;

        /**
         * Check the json type
         */
        virtual bool isObject() const;
        virtual bool isArray() const;
        virtual bool isNumber() const;
        virtual bool isString() const;
        virtual bool isTrue() const;
        virtual bool isFalse() const;
        virtual bool isNull() const;

        virtual bool isBool() const;
        virtual bool isNone() const;

    public:
        /**
         * Get value, throw exception if type is incorrect
         */
        virtual const chen::json::object& getObject() const;
        virtual const chen::json::array& getArray() const;
        virtual double getNumber() const;
        virtual int getInteger() const;  // maybe losing precision
        virtual const std::string& getString() const;
        virtual bool getBool() const;

        /**
         * Convert value to the desired type as possible
         */
        virtual chen::json::object toObject() const;
        virtual chen::json::array toArray() const;
        virtual double toNumber() const;
        virtual int toInteger() const;  // maybe losing precision
        virtual std::string toString() const;
        virtual bool toBool() const;

    protected:
        /**
         * Throw syntax exception
         */
        template <class InputIterator>
        void exception(InputIterator &cur, InputIterator &end) const;

        /**
         * Filter the beginning white spaces
         */
        template <class InputIterator>
        void filter(InputIterator &cur, InputIterator &end, bool require) const;

        /**
         * Skip current character
         */
        template <class InputIterator>
        char forward(InputIterator &cur, InputIterator &end) const;

        /**
         * Advance to the next character
         */
        template <class InputIterator>
        char advance(InputIterator &cur, InputIterator &end, bool require) const;

        /**
         * Decode specific type
         */
        template <class InputIterator>
        void decode(chen::json &out, InputIterator &cur, InputIterator &end) const;

        template <class InputIterator>
        void decode(chen::json::object &out, InputIterator &cur, InputIterator &end) const;

        template <class InputIterator>
        void decode(chen::json::array &out, InputIterator &cur, InputIterator &end) const;

        template <class InputIterator>
        void decode(double &out, InputIterator &cur, InputIterator &end) const;

        template <class InputIterator>
        void decode(std::string &out, InputIterator &cur, InputIterator &end) const;

        template <class InputIterator>
        void decode(bool v, InputIterator &cur, InputIterator &end) const;

        template <class InputIterator>
        void decode(std::nullptr_t, InputIterator &cur, InputIterator &end) const;

        /**
         * Encode specific type
         */
        virtual void encode(std::string &output, std::size_t space, std::size_t &indent) const;

        virtual void encode(const chen::json::object &v,
                            std::string &output,
                            std::size_t space,
                            std::size_t &indent) const;
        virtual void encode(const chen::json::array &v,
                            std::string &output,
                            std::size_t space,
                            std::size_t &indent) const;
        virtual void encode(double v, std::string &output) const;
        virtual void encode(const std::string &v, std::string &output) const;
        virtual void encode(bool v, std::string &output) const;
        virtual void encode(std::nullptr_t, std::string &output) const;

    protected:
        JsonType _type = JsonType::None;
        JsonData _data = {nullptr};
    };
}

// decode
template <class InputIterator>
void chen::json::decode(InputIterator cur, InputIterator end)
{
    // trim left spaces
    this->filter(cur, end, true);

    // decode item
    chen::json item;
    this->decode(item, cur, end);

    // trim right spaces
    this->filter(cur, end, false);

    // should reach end
    if (cur != end)
        this->exception(cur, end);

    // assign item
    *this = std::move(item);
}

// exception
template <class InputIterator>
void chen::json::exception(InputIterator &cur, InputIterator &end) const
{
    if (cur == end)
        throw error_syntax("json unexpected end of input");
    else
        throw error_syntax(str::format("json unexpected token '%c'", *cur));
}

// filter
template <class InputIterator>
void chen::json::filter(InputIterator &cur, InputIterator &end, bool require) const
{
    for (; (cur != end) && std::isspace(*cur); ++cur)
        ;

    if (require && (cur == end))
        this->exception(cur, end);
}

// forward
template <class InputIterator>
char chen::json::forward(InputIterator &cur, InputIterator &end) const
{
    if (cur == end)
        this->exception(cur, end);

    ++cur;
    return cur == end ? static_cast<char>(-1) : *cur;
}

// todo remove this
// advance
template <class InputIterator>
char chen::json::advance(InputIterator &cur, InputIterator &end, bool require) const
{
    if (cur == end)
    {
        if (require)
            this->exception(cur, end);
        else
            return static_cast<char>(-1);
    }

    char ch = *cur;

    if (require && (cur == end))
        this->exception(cur, end);

    return ch;
}

// decode type
template <class InputIterator>
void chen::json::decode(chen::json &out, InputIterator &cur, InputIterator &end) const
{
    auto ch = this->advance(cur, end, true);

    switch (ch)
    {
        case '{':  // object
        {
            chen::json::object o;
            this->decode(o, cur, end);
            out = std::move(o);
        }
            break;

        case '[':  // array
        {
            chen::json::array a;
            this->decode(a, cur, end);
            out = std::move(a);
        }
            break;

        case '"':  // string
        {
            std::string s;
            this->decode(s, cur, end);
            out = std::move(s);
        }
            break;

        case 't':  // true
            this->decode(true, cur, end);
            out = true;
            break;

        case 'f':  // false
            this->decode(false, cur, end);
            out = false;
            break;

        case 'n':  // null
            this->decode(nullptr, cur, end);
            out = nullptr;
            break;

        default:
            if (std::isdigit(ch) || (ch == '-'))  // number
            {
                double d = 0;
                this->decode(d, cur, end);
                out = d;
            }
            else
            {
                this->exception(cur, end);
            }
            break;
    }
}

template <class InputIterator>
void chen::json::decode(chen::json::object &out, InputIterator &cur, InputIterator &end) const
{
    if (this->advance(cur, end, true) != '{')
        this->exception(cur, end);

    this->forward(cur, end);

    while (cur != end)
    {
        // skip spaces
        this->filter(cur, end, true);

        auto ch = this->advance(cur, end, true);
        if (ch == '}')
            break;

        // find key
        if (ch != '"')
            this->exception(cur, end);

        std::string key;
        this->decode(key, cur, end);

        // find separator
        this->filter(cur, end, true);

        if (this->advance(cur, end, true) != ':')  // separator
            this->exception(cur, end);

        this->forward(cur, end);
        this->filter(cur, end, true);

        // find value
        chen::json item;
        this->decode(item, cur, end);

        out[std::move(key)] = std::move(item);

        // find comma or ending
        this->filter(cur, end, true);

        ch = this->advance(cur, end, true);

        if (ch == ',')
        {
            this->forward(cur, end);
            this->filter(cur, end, true);

            if (this->advance(cur, end, true) == '}')
                this->exception(cur, end);
        }
        else if (ch == '}')
        {
            break;
        }
        else
        {
            this->exception(cur, end);
        }
    }

    if (cur == end)
        this->exception(cur, end);

    this->forward(cur, end);
}

template <class InputIterator>
void chen::json::decode(chen::json::array &out, InputIterator &cur, InputIterator &end) const
{
    if (this->advance(cur, end, true) != '[')
        this->exception(cur, end);

    this->forward(cur, end);

    while (cur != end)
    {
        this->filter(cur, end, true);

        if (this->advance(cur, end, true) == ']')
            break;

        // find value
        chen::json item;
        this->decode(item, cur, end);

        out.push_back(std::move(item));

        // find comma or ending
        this->filter(cur, end, true);

        auto ch = this->advance(cur, end, true);

        if (ch == ',')
        {
            this->forward(cur, end);
            this->filter(cur, end, true);

            if (this->advance(cur, end, true) == ']')
                this->exception(cur, end);
        }
        else if (ch == ']')
        {
            break;
        }
        else
        {
            this->exception(cur, end);
        }
    }

    if (cur == end)
        this->exception(cur, end);

    this->forward(cur, end);
}

template <class InputIterator>
void chen::json::decode(double &out, InputIterator &cur, InputIterator &end) const
{
    auto ch = this->advance(cur, end, true);
    if ((ch != '-') && !std::isdigit(ch))
        this->exception(cur, end);

    std::string str(1, ch);

    this->forward(cur, end);

    // must begin with '-' or '1' ~ '9'
    if (str[0] == '-')
    {
        ch = this->advance(cur, end, true);

        if (ch == '0')
        {
            str += ch;

            this->forward(cur, end);

            if (std::isdigit(this->advance(cur, end, false)))
            {
                // todo back to 0
//                stream.unget();
                this->exception(cur, end);
            }
        }
        else if (!std::isdigit(ch))
        {
            this->exception(cur, end);
        }
    }
    else if ((str[0] == '0') && std::isdigit(this->advance(cur, end, false)))
    {
        // if the first char is '0', then the whole number must be 0
        this->exception(cur, end);
    }

    // collect integer parts
    while (std::isdigit(ch = this->advance(cur, end, false)))
    {
        str += ch;
        this->forward(cur, end);
    }

    // collect decimal parts
    if ((ch = this->advance(cur, end, false)) == '.')
    {
        str += ch;
        this->forward(cur, end);

        // first
        if (std::isdigit(ch = this->advance(cur, end, true)))
        {
            str += ch;
            this->forward(cur, end);
        }
        else
        {
            this->exception(cur, end);
        }

        // digits
        while (std::isdigit(ch = this->advance(cur, end, false)))
        {
            str += ch;
            this->forward(cur, end);
        }
    }

    // 'e' or 'E'
    ch = this->advance(cur, end, false);

    if ((ch == 'e') || (ch == 'E'))
    {
        str += ch;
        this->forward(cur, end);

        // '+', '-' or digits
        ch = this->advance(cur, end, true);

        if ((ch == '+') || (ch == '-') || std::isdigit(ch))
        {
            str += ch;
            this->forward(cur, end);

            // first
            if (!std::isdigit(ch))
            {
                if (std::isdigit(ch = this->advance(cur, end, true)))
                {
                    str += ch;
                    this->forward(cur, end);
                }
                else
                {
                    this->exception(cur, end);
                }
            }

            // last
            while (std::isdigit(ch = this->advance(cur, end, false)))
            {
                str += ch;
                this->forward(cur, end);
            }
        }
        else
        {
            this->exception(cur, end);
        }
    }

    // check if number is overflow
    double d = std::atof(str.c_str());

    if (std::isinf(d))
        throw error_syntax("json number is overflow: " + str);
    else if (std::isnan(d))
        throw error_syntax("json number is incorrect: " + str);

    out = d;
}

template <class InputIterator>
void chen::json::decode(std::string &out, InputIterator &cur, InputIterator &end) const
{
    if (this->advance(cur, end, true) != '"')
        this->exception(cur, end);

    this->forward(cur, end);

    auto ch = this->advance(cur, end, true);

    while (ch != '"')
    {
        // control characters must use escape
        if ((ch >= 0) && (ch <= 31))  // see ASCII
            throw error_syntax("json control character is not escaped");

        // unescape characters
        if (ch == '\\')
        {
            this->forward(cur, end);

            ch = this->advance(cur, end, true);

            switch (ch)
            {
                case '"':
                case '\\':
                case '/':
                    out += ch;
                    ch = this->forward(cur, end);
                    break;

                case 'b':
                    out += '\b';
                    ch = this->forward(cur, end);
                    break;

                case 'f':
                    out += '\f';
                    ch = this->forward(cur, end);
                    break;

                case 'n':
                    out += '\n';
                    ch = this->forward(cur, end);
                    break;

                case 'r':
                    out += '\r';
                    ch = this->forward(cur, end);
                    break;

                case 't':
                    out += '\t';
                    ch = this->forward(cur, end);
                    break;

                case 'u':
                {
                    this->forward(cur, end);

                    // handle unicode character
                    char unicode[5] = {0};

                    for (auto i = 0; i < 4; ++i)
                    {
                        ch = this->advance(cur, end, true);
                        this->forward(cur, end);

                        if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
                            unicode[i] = ch;  // char must in range of '0' ~ '9', 'a' ~ 'f', 'A' ~ 'F'
                        else
                            this->exception(cur, end);
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
                        throw error_syntax(str::format("json invalid unicode char \\u%s", unicode));
                    }
                }
                    break;

                default:
                    this->exception(cur, end);
            }
        }
        else
        {
            out += ch;
            this->forward(cur, end);
            ch = this->advance(cur, end, true);
        }
    }

    this->advance(cur, end, true);
    this->forward(cur, end);
}

template <class InputIterator>
void chen::json::decode(bool v, InputIterator &cur, InputIterator &end) const
{
    static const std::string t("true");
    static const std::string f("false");

    auto &s = v ? t : f;

    for (std::size_t i = 0, l = s.size(); i < l; ++i)
    {
        if (this->advance(cur, end, true) != s[i])
            this->exception(cur, end);

        this->forward(cur, end);
    }
}

template <class InputIterator>
void chen::json::decode(std::nullptr_t, InputIterator &cur, InputIterator &end) const
{
    static const std::string n("null");

    for (std::size_t i = 0, l = n.size(); i < l; ++i)
    {
        if (this->advance(cur, end, true) != n[i])
            this->exception(cur, end);

        this->forward(cur, end);
    }
}