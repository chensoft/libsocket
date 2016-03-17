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

#include <string>
#include <vector>
#include <map>
#include <chen/tool/any.hpp>

namespace chen
{
    class json
    {
    public:
        typedef std::vector<chen::json> array;
        typedef std::map<std::string, chen::json> object;

        enum class JsonType {Object, Array, Number, String, True, False, Null};

    public:
        json() = default;

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

        virtual ~json() = default;

        /**
         * Copy & Move
         */
        json& operator=(const json &o);
        json& operator=(json &&o);

    public:
        /**
         * Decode the json text, throw exception if found error
         */
        virtual void decode(const std::string &text);

        /**
         * Encode the json object to a string
         * @param pretty enable pretty print
         */
        virtual std::string encode(bool pretty = false) const;

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
        // todo use const reference, don't use any type
        /**
         * Get the json value
         * convert to the desired type as possible
         */
        virtual chen::json::object asObject() const;
        virtual chen::json::array asArray() const;
        virtual double asNumber() const;
        virtual std::string asString() const;
        virtual bool asBool() const;

    public:
        /**
         * Json parse and stringify helper
         * use encode and decode internally
         */
        static chen::json parse(const std::string &text);
        static std::string stringify(const chen::json &json, bool pretty = false);

    protected:
        JsonType _type = JsonType::Null;
        chen::any _data;
    };
}