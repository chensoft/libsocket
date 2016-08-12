/**
 * Created by Jian Chen
 * @since  2016.06.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/data/json.hpp>
#include <chen/base/str.hpp>
#include <chen/sys/fs.hpp>
#include <gtest/gtest.h>
#include "../../conf.hpp"

using namespace chen;

TEST(DataJsonTest, Type)
{
    json json_object(json::Type::Object);
    json json_array(json::Type::Array);
    json json_number(json::Type::Number);
    json json_string(json::Type::String);
    json json_true(json::Type::True);
    json json_false(json::Type::False);

    json::object unused_o = json_object;
    json::array unused_a  = json_array;

    double unused_d = json_number;
    std::int8_t   unused_i8  = json_number;
    std::uint8_t  unused_u8  = json_number;
    std::int16_t  unused_i16 = json_number;
    std::uint16_t unused_u16 = json_number;
    std::int32_t  unused_i32 = json_number;
    std::uint32_t unused_u32 = json_number;
    std::int64_t  unused_i64 = json_number;
    std::uint64_t unused_u64 = json_number;

    std::string unused_s = json_string;
    bool unused_b = json_true;
    
    EXPECT_EQ(0.0, unused_d);
    EXPECT_EQ(0, unused_i8);
    EXPECT_EQ(0, unused_u8);
    EXPECT_EQ(0, unused_i16);
    EXPECT_EQ(0, unused_u16);
    EXPECT_EQ(0, unused_i32);
    EXPECT_EQ(0, unused_u32);
    EXPECT_EQ(0, unused_i64);
    EXPECT_EQ(0, unused_u64);
    EXPECT_EQ(true, unused_b);

    json_object.getObject().clear();
    json_array.getArray().clear();
    json_number.getNumber() = 115;
    json_string.getString() = "123";

    EXPECT_TRUE(json_object.toObject().empty());
    EXPECT_TRUE(json_array.toArray().empty());

    EXPECT_TRUE(json(unused_o).getObject().empty());
    EXPECT_TRUE(json(unused_a).getArray().empty());
    EXPECT_TRUE(json(unused_s).getString().empty());

    json assign_o;
    json assign_a;
    json assign_s;
    json assign_d;

    assign_o = unused_o;
    assign_a = unused_a;
    assign_s = unused_s;
    assign_d = 115u;

    EXPECT_TRUE(assign_o.getObject().empty());
    EXPECT_TRUE(assign_a.getArray().empty());
    EXPECT_TRUE(assign_s.getString().empty());
    EXPECT_EQ(115u, assign_d.getUnsigned());

    EXPECT_EQ(123, json_string.toNumber());
    EXPECT_EQ(123, json_string.toInteger());
    EXPECT_EQ(123, json_string.toUnsigned());
    EXPECT_EQ("115", json_number.toString());
    EXPECT_TRUE(json_object.toBool());

    EXPECT_TRUE(json(json::Type::None).isNone());
    EXPECT_TRUE(json_object.isObject());
    EXPECT_TRUE(json_array.isArray());
    EXPECT_TRUE(json_number.isNumber());
    EXPECT_TRUE(json_string.isString());
    EXPECT_TRUE(json_true.isTrue());
    EXPECT_TRUE(json_false.isFalse());
    EXPECT_TRUE(json(json::Type::Null).isNull());

    EXPECT_TRUE(json().isNone());
    EXPECT_TRUE(json(json::object()).isObject());
    EXPECT_TRUE(json(json::array()).isArray());
    EXPECT_TRUE(json(115).isNumber());
    EXPECT_TRUE(json(115u).isNumber());
    EXPECT_TRUE(json(0.715002586).isNumber());
    EXPECT_TRUE(json("Jian Chen").isString());
    EXPECT_TRUE(json(std::string("Jian Chen")).isString());
    EXPECT_TRUE(json(true).isTrue());
    EXPECT_TRUE(json(false).isFalse());
    EXPECT_TRUE(json(true).isBool());
    EXPECT_TRUE(json(nullptr).isNull());

    // parse special string
    EXPECT_TRUE(json::parse("   ").isNone());
    EXPECT_THROW(json::parse("-"), json::error);
    EXPECT_THROW(json::parse("-03"), json::error);
    EXPECT_THROW(json::parse("-0."), json::error);
    EXPECT_THROW(json::parse("1e123456789"), json::error);
    EXPECT_THROW(json::parse("\"ab\\"), json::error);
    EXPECT_THROW(json::parse("\"ab\\u9A"), json::error);
    EXPECT_THROW(json::parse("\"ab\\u9A@@"), json::error);
    EXPECT_THROW(json::parse("\"\\uD83D\\uDE00\""), json::error);
    EXPECT_THROW(json::parse("nul"), json::error);
}

TEST(DataJsonTest, Validate)
{
    if (conf::data.empty())
        return ::testing::internal::ColoredPrintf(::testing::internal::COLOR_YELLOW, "warning: you didn't specify test data folder, skip json test\n\n");

    // fail
    for (int i = 1; i <= 33; ++i)
    {
        if (i == 18)  // I don't think too deep is an error
            continue;

        EXPECT_THROW(json::validate(conf::data + str::format("/json/fail%d.json", i), true), json::error);
    }

    // pass
    for (int j = 1; j <= 3; ++j)
    {
        EXPECT_NO_THROW(json::validate(conf::data + str::format("/json/pass%d.json", j), true));
    }

    EXPECT_NO_THROW(json::parse(conf::data + "/json/pass1.json", true));

    // exist
    EXPECT_THROW(json::parse(conf::data + "/json/not_exist.json", true), json::error);

    // equal
    std::string text = R"([
    1,
    2,
    3,
    false,
    null,
    {
        "key": "val-\"\\\/\b\f\n\r\tchars"
    }
])";

    auto parse = json::parse(text);
    EXPECT_EQ(text, json::stringify(parse, 4));

    // error
    text = "{unquoted_key: \"keys must be quoted\"}";
    EXPECT_THROW(json::parse(text), json::error);

    try
    {
        json::parse(text);
    }
    catch (const json::error &e)
    {
        EXPECT_EQ(1, e.position);
    }
}