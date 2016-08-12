/**
  * Created by Jian Chen
  * @since  2016.05.27
  * @author Jian Chen <admin@chensoft.com>
  * @link   http://chensoft.com
  */
#include <chen/base/utf8.hpp>
#include <chen/sys/fs.hpp>
#include <gtest/gtest.h>
#include "../../conf.hpp"

using namespace chen;

TEST(BaseUtf8Test, General)
{
    if (conf::data.empty())
        return ::testing::internal::ColoredPrintf(::testing::internal::COLOR_YELLOW, "warning: you didn't specify test data folder, skip utf8 test\n\n");

    // these txt files contains utf-8 chars, each char's size is 1byte ~ 6byte
    auto bom   = fs::read(conf::data + "/utf8/bom.txt");
    auto sub   = fs::read(conf::data + "/utf8/sub.txt");
    auto code  = fs::read(conf::data + "/utf8/code.txt");
    auto plain = fs::read(conf::data + "/utf8/plain.txt");

    EXPECT_EQ(11, utf8::count(plain));
    EXPECT_EQ(sub, utf8::substr(plain, 0, 7));
    EXPECT_EQ("", utf8::substr(plain, 5, 0));
    EXPECT_TRUE(utf8::bom(bom));
    EXPECT_FALSE(utf8::bom(plain));

    EXPECT_EQ("a", utf8::convert(0x0061));
    EXPECT_EQ("©", utf8::convert(0x00A9));
    EXPECT_EQ(code, utf8::convert(0x9648));
}