/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/str.hpp>
#include <gtest/gtest.h>

TEST(BaseStrTest, General)
{
    // format
    EXPECT_EQ("", chen::str::format(""));
    EXPECT_EQ("plain", chen::str::format("plain"));
    EXPECT_EQ("year 2016", chen::str::format("year %d", 2016));
    EXPECT_EQ(8, chen::str::format("chen%s", "jian").size());

    // check
    EXPECT_TRUE(chen::str::equal("chen", 4, "chen", 4));
    EXPECT_TRUE(chen::str::prefix("chensoft", "chen"));
    EXPECT_FALSE(chen::str::prefix("chensoft", "soft"));
    EXPECT_FALSE(chen::str::prefix("chen", "chensoft"));
    EXPECT_TRUE(chen::str::suffix("chensoft", "soft"));
    EXPECT_FALSE(chen::str::suffix("chensoft", "chen"));
    EXPECT_FALSE(chen::str::suffix("chen", "chensoft"));
    EXPECT_TRUE(chen::str::contain("libchen", "lib"));

    // count
    EXPECT_EQ(3, chen::str::count("I scream, you scream, we all scream for ice-cream!", "scream"));

    // split & join
    std::vector<std::string> split({"10", "0", "0", "1"});
    EXPECT_EQ(split, chen::str::split("10.0.0.1", "."));
    EXPECT_EQ(split, chen::str::split("10->0->0->1", "->"));
    EXPECT_EQ(std::vector<std::string>{"10"}, chen::str::split("10", "."));
    EXPECT_EQ(std::vector<std::string>{}, chen::str::split("", "."));
    EXPECT_EQ("10.0.0.1", chen::str::join(split, "."));
    EXPECT_EQ("", chen::str::join(std::vector<std::string>(), "."));

    // replace
    EXPECT_EQ("I'm a replace string", chen::str::replace("I'm a search string", "search", "replace"));
    EXPECT_EQ("I'm a  string", chen::str::remove("I'm a replace string", "replace"));

    EXPECT_EQ("I'm a replace string, and another search string", chen::str::replace("I'm a search string, and another search string", "search", "replace", false));
    EXPECT_EQ("I'm a replace string, and another replace string", chen::str::replace("I'm a search string, and another search string", "search", "replace"));

    // trim
    EXPECT_EQ("so many spaces  ", chen::str::ltrim("  so many spaces  "));
    EXPECT_EQ("  so many spaces", chen::str::rtrim("  so many spaces  "));
    EXPECT_EQ("so many spaces", chen::str::trim("  so many spaces  "));

    // transform
    EXPECT_EQ("", chen::str::capitalize(""));
    EXPECT_EQ("AbCDe", chen::str::capitalize("abCDe"));
    EXPECT_EQ("abcde", chen::str::lowercase("abCDe"));
    EXPECT_EQ("ABCDE", chen::str::uppercase("abCDe"));

    // levenshtein
    EXPECT_EQ(2, chen::str::levenshtein("pull", 4, "push", 4));
}