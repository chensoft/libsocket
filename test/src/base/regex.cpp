/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/regex.hpp>
#include <gtest/gtest.h>
#include "../../conf.hpp"

TEST(BaseRegexTest, General)
{
    try
    {
        std::string pattern(R"(^\d+$)");
        EXPECT_TRUE(chen::regex::match("12345", pattern));

        pattern = R"(soft\.com)";
        EXPECT_EQ("chenjian", chen::regex::replace("chensoft.com", pattern, "jian"));

        EXPECT_EQ("0.0.0.127", chen::regex::expand("abc.127", "(?:^|\\D\\.)(\\d+)$", "0.0.0.$1"));

        auto group = chen::regex::group("127.0.0.1", R"((\d+)\.(\d+)\.(\d+)\.(\d+))");
        EXPECT_EQ(5, group.size());
        EXPECT_EQ("127", group[1]);
        EXPECT_EQ("0", group[2]);
        EXPECT_EQ("0", group[3]);
        EXPECT_EQ("1", group[4]);
    }
    catch (const std::exception&)
    {
        ::testing::internal::ColoredPrintf(::testing::internal::COLOR_YELLOW, "warning: regex support is incomplete on your compiler\n\n");
    }
}