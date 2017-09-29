/**
 * Created by Jian Chen
 * @since  2016.06.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/data/ini.hpp"
#include "chen/base/str.hpp"
#include "chen/sys/fs.hpp"
#include "gtest/gtest.h"
#include "../conf.hpp"

TEST(DataIniTest, General)
{
    using chen::ini;
    using chen::str;

    if (conf::data.empty())
        return ::testing::internal::ColoredPrintf(::testing::internal::COLOR_YELLOW, "warning: you didn't specify test data folder, skip ini test\n\n");

    // fail
    for (int i = 1; i <= 5; ++i)
    {
        EXPECT_THROW(ini::parse(conf::data + str::format("/ini/fail%d.ini", i), true), ini::error);
    }

    // pass
    for (int j = 1; j <= 4; ++j)
    {
        EXPECT_NO_THROW(ini::parse(conf::data + str::format("/ini/pass%d.ini", j), true));
    }

    // equal(usage of block text in C++11)
    std::string text = R"([section]
key="simple \\\0\a\b\t\r\n\:\"\;\#value")";

    auto parse = ini::parse(text);
    EXPECT_EQ(text, ini::stringify(parse));

    // parse special string
    EXPECT_TRUE(ini::parse("   ").empty());
    EXPECT_THROW(ini::parse("ab=\\x9A"), ini::error);
    EXPECT_THROW(ini::parse("ab=\\x9A@@"), ini::error);
    EXPECT_THROW(ini::parse("emoji=\\xD83D\\xDE00"), ini::error);

    // error
    text = R"([section
key="section is not enclosed")";

    EXPECT_THROW(ini::parse(text), ini::error);

    try
    {
        ini::parse(text);
    }
    catch (const ini::error &e)
    {
        EXPECT_EQ(8u, e.position);
    }
}