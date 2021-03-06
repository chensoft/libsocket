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

#include "chen/base/iterator.hpp"
#include <unordered_map>
#include <exception>
#include <string>
#include <vector>

namespace chen
{
    class json
    {
    public:
        typedef chen::input_iterator<const std::uint8_t, const std::uint8_t> iterator;  // it's an input iterator

        typedef std::unordered_map<std::string, json> object;
        typedef std::vector<json> array;

        enum class Type {None, Object, Array, Number, String, True, False, Null};

        union Data
        {
            json::object *o;
            json::array *a;
            double d;
            std::string *s;
        };

        class error : public std::runtime_error
        {
        public:
            static const std::size_t npos = static_cast<std::size_t>(-1);

            explicit error(const std::string &what) : std::runtime_error(what) {}
            error(const std::string &what, std::size_t position) : std::runtime_error(what), position(position) {}

        public:
            std::size_t position = npos;  // only valid when syntax error occur
        };

    public:
        /**
         * Constructor
         */
        json() = default;
        json(json::Type type);

        json(const json &o);
        json(json &&o);

        json(const json::object &v);
        json(json::object &&v);

        json(const json::array &v);
        json(json::array &&v);

        json(double v);
        json(std::int32_t v);
        json(std::uint32_t v);

        json(const std::string &v);
        json(std::string &&v);
        json(const char *v);

        json(bool v);

        json(std::nullptr_t);

        ~json();

        /**
         * Assignment
         */
        json& operator=(std::nullptr_t);

        json& operator=(const json &o);
        json& operator=(json &&o);

        json& operator=(const json::object &v);
        json& operator=(json::object &&v);

        json& operator=(const json::array &v);
        json& operator=(json::array &&v);

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
         * accept any iterator that satisfy the requirement of the input iterator
         */
        static json parse(const std::string &text, bool file = false);
        static json parse(iterator cur, iterator end);

        /**
         * Json stringify
         */
        static std::string stringify(const json &json, std::size_t space = 0);

    public:
        /**
         * Validate the json, throw exception if has error
         * according to the rfc4627, a json text must be a serialized object or array
         */
        static void validate(const std::string &text, bool file = false);
        static void validate(iterator cur, iterator end);

    public:
        /**
         * Get the json type
         */
        json::Type type() const;

        /**
         * Check the json type
         */
        bool isObject() const;
        bool isArray()  const;
        bool isNumber() const;
        bool isString() const;
        bool isTrue()   const;
        bool isFalse()  const;
        bool isNull()   const;

        bool isBool() const;
        bool isNone() const;

    public:
        /**
         * Get value, throw exception if type is incorrect
         */
        const json::object& getObject() const;
        const json::array& getArray() const;
        double getNumber() const;
        std::int32_t getInteger() const;
        std::uint32_t getUnsigned() const;
        const std::string& getString() const;
        bool getBool() const;

        /**
         * Get value via operators
         * e.g:
         * >> json json(115);
         * >> int val = json;
         */
        operator json::object() const;
        operator json::array() const;
        operator double() const;
        operator std::int8_t() const;  // maybe losing precision
        operator std::uint8_t() const;
        operator std::int16_t() const;
        operator std::uint16_t() const;
        operator std::int32_t() const;
        operator std::uint32_t() const;
        operator std::int64_t() const;  // json can't represent native 64bits integer
        operator std::uint64_t() const;
        operator std::string() const;
        operator bool() const;

        /**
         * Get value, user can modify its content
         * @caution the integer method does not exist because we store number as double
         * @caution the bool method does not exist because modify it will result in wrong type
         */
        json::object& getObject();
        json::array& getArray();
        double& getNumber();
        std::string& getString();

        /**
         * Convert value to the desired type as possible
         */
        json::object toObject() const;
        json::array toArray() const;
        double toNumber() const;
        std::int32_t toInteger() const;
        std::uint32_t toUnsigned() const;
        std::string toString() const;
        bool toBool() const;

        /**
         * Clear the internal state
         */
        void clear();

    protected:
        /**
         * Throw syntax exception
         */
        static void exception(const iterator &beg, iterator &cur, iterator &end);

        /**
         * Advance to the next non-whitespace character
         */
        static bool advance(const iterator &beg, iterator &cur, iterator &end, bool require = true);

        /**
         * Decode specific type
         */
        static void decode(json &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(json::object &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(json::array &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(double &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(std::string &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(bool v, const iterator &beg, iterator &cur, iterator &end);
        static void decode(std::nullptr_t, const iterator &beg, iterator &cur, iterator &end);

        /**
         * Encode specific type
         */
        static void encode(const json &v, std::size_t space, std::string &output, std::size_t &indent);
        static void encode(const json::object &v, std::size_t space, std::string &output, std::size_t &indent);
        static void encode(const json::array &v, std::size_t space, std::string &output, std::size_t &indent);
        static void encode(double v, std::string &output);
        static void encode(const std::string &v, std::string &output);
        static void encode(bool v, std::string &output);
        static void encode(std::nullptr_t, std::string &output);

    protected:
        Type _type = Type::None;
        Data _data = {nullptr};
    };
}