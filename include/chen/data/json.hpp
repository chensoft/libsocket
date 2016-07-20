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

#include <chen/base/iterator.hpp>
#include <unordered_map>
#include <exception>
#include <string>
#include <vector>

namespace chen
{
    class json
    {
    public:
        typedef chen::input_iterator<const char, const char> iterator;

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
         * accept any iterator that satisfy the requirement of the input iterator
         */
        static chen::json parse(const std::string &text, bool file = false);
        static chen::json parse(iterator cur, iterator end);

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
        static void validate(iterator cur, iterator end);

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
        virtual std::int32_t getInteger() const;
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
        virtual std::int32_t toInteger() const;
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
        static void exception(const iterator &beg, iterator &cur, iterator &end);

        /**
         * Advance to the next non-whitespace character
         */
        static bool advance(const iterator &beg, iterator &cur, iterator &end, bool require = true);

        /**
         * Decode specific type
         */
        static void decode(chen::json &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(chen::json::object &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(chen::json::array &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(double &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(std::string &out, const iterator &beg, iterator &cur, iterator &end);
        static void decode(bool v, const iterator &beg, iterator &cur, iterator &end);
        static void decode(std::nullptr_t, const iterator &beg, iterator &cur, iterator &end);

        /**
         * Encode specific type
         */
        static void encode(const chen::json &v, std::size_t space, std::string &output, std::size_t &indent);
        static void encode(const chen::json::object &v, std::size_t space, std::string &output, std::size_t &indent);
        static void encode(const chen::json::array &v, std::size_t space, std::string &output, std::size_t &indent);
        static void encode(double v, std::string &output);
        static void encode(const std::string &v, std::string &output);
        static void encode(bool v, std::string &output);
        static void encode(std::nullptr_t, std::string &output);

    protected:
        Type _type = Type::None;
        Data _data = {nullptr};
    };
}