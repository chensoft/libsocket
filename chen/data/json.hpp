/**
 * A tiny json parser
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   http://www.json.org
 * -----------------------------------------------------------------------------
 * A JSON value can be an object, array, number, string, true, false, or null
 * we represent the number as double type, like the javascript
 * -----------------------------------------------------------------------------
 * Usage:  // todo
 * >> chen::cmd cmd;
 * >>
 * >> try
 * >> {
 * >>     cmd.define("help", "h", "show help", false);
 * >>     cmd.define("port", "p", "server port (default: 53)", 53);
 * >>     cmd.define("zone", "z", "zone folder", "");
 * >>
 * >>     cmd.parse(argc, argv);
 * >>
 * >>     bool help = cmd.boolVal("help");
 * >> }
 * >> catch (const chen::cmd::error_parse &e)
 * >> {
 * >>     std::cout << cmd.usage(e) << std::endl;
 * >> }
 */
#pragma once

#include <exception>
#include <string>
#include <vector>
#include <map>
// todo support initialize list
// todo benchmark with js engine

namespace chen
{
    class json
    {
    public:
        typedef std::map<std::string, chen::json> object;
        typedef std::vector<chen::json> array;

        enum class JsonType {Object, Array, Number, String, True, False, Null};

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
            explicit error_syntax(const std::string &what, std::size_t line, std::size_t column) : chen::json::error(what) {}

            std::size_t line   = 0;
            std::size_t column = 0;
        };

    public:
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
         * Copy & Move
         */
        json& operator=(const json &o);
        json& operator=(json &&o);

    public:
        /**
         * Json parse and stringify helper
         * use encode and decode internally
         */
        static chen::json parse(const std::string &text);
        static std::string stringify(const chen::json &json,
                                     std::size_t space = 0);

    public:
        // todo add stream support, read chars from stream
        // todo add file support, read from file stream
        /**
         * Decode the json text, throw exception if found error
         */
        virtual void decode(const std::string &text);

        /**
         * Encode the json object to a string
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

        // todo add set value

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
         * Encode helper
         */
        virtual void encode(std::string &output,
                            std::size_t space,
                            std::size_t &indent) const;

        /**
         * Encode specific type
         */
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
        JsonType _type = JsonType::Null;
        JsonData _data = {nullptr};
    };
}