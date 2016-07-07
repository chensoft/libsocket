/**
 * Created by Jian Chen
 * @since  2016.06.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(DataJsonTest, Type)
{
    chen::json json_object(chen::json::Type::Object);
    chen::json json_array(chen::json::Type::Array);
    chen::json json_number(chen::json::Type::Number);
    chen::json json_string(chen::json::Type::String);
    chen::json json_true(chen::json::Type::True);
    chen::json json_false(chen::json::Type::False);

    chen::json::object unused_o = json_object;
    chen::json::array unused_a  = json_array;

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

    json_object.getObject().clear();
    json_array.getArray().clear();
    json_number.getNumber() = 115;
    json_string.getString() = "123";

    EXPECT_TRUE(json_object.toObject().empty());
    EXPECT_TRUE(json_array.toArray().empty());

    EXPECT_EQ(123, json_string.toNumber());
    EXPECT_EQ(123, json_string.toInteger());
    EXPECT_EQ(123, json_string.toUnsigned());
    EXPECT_EQ("115", json_number.toString());
    EXPECT_TRUE(json_object.toBool());

    EXPECT_TRUE(chen::json(chen::json::Type::None).isNone());
    EXPECT_TRUE(json_object.isObject());
    EXPECT_TRUE(json_array.isArray());
    EXPECT_TRUE(json_number.isNumber());
    EXPECT_TRUE(json_string.isString());
    EXPECT_TRUE(json_true.isTrue());
    EXPECT_TRUE(json_false.isFalse());
    EXPECT_TRUE(chen::json(chen::json::Type::Null).isNull());

    EXPECT_TRUE(chen::json().isNone());
    EXPECT_TRUE(chen::json(chen::json::object()).isObject());
    EXPECT_TRUE(chen::json(chen::json::array()).isArray());
    EXPECT_TRUE(chen::json(115).isNumber());
    EXPECT_TRUE(chen::json(115u).isNumber());
    EXPECT_TRUE(chen::json(0.715002586).isNumber());
    EXPECT_TRUE(chen::json("Jian Chen").isString());
    EXPECT_TRUE(chen::json(std::string("Jian Chen")).isString());
    EXPECT_TRUE(chen::json(true).isTrue());
    EXPECT_TRUE(chen::json(false).isFalse());
    EXPECT_TRUE(chen::json(true).isBool());
    EXPECT_TRUE(chen::json(nullptr).isNull());
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

        EXPECT_THROW(chen::json::validate(chen::fs::read(conf::data + chen::str::format("/json/fail%d.json", i))), chen::json::error);
    }

    // pass
    for (int j = 1; j <= 3; ++j)
    {
        EXPECT_NO_THROW(chen::json::validate(conf::data + chen::str::format("/json/pass%d.json", j), true));
    }

    EXPECT_NO_THROW(chen::json::parse(conf::data + "/json/pass1.json", true));

    // exist
    EXPECT_THROW(chen::json::parse(conf::data + "/json/not_exist.json", true), chen::json::error);

    // equal
    auto text  = R"([
    1,
    2,
    3,
    false,
    null,
    {
        "key": "val-\"\\\/\b\f\n\r\tchars"
    }
])";

    auto parse = chen::json::parse(text);
    EXPECT_EQ(text, chen::json::stringify(parse, 4));
}