/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/str.hpp>
#include <gtest/gtest.h>

using namespace chen;

TEST(BaseStrTest, General)
{
    // format
    EXPECT_EQ("", str::format(""));
    EXPECT_EQ("plain", str::format("plain"));
    EXPECT_EQ("year 2016", str::format("year %d", 2016));
    EXPECT_EQ(8, str::format("chen%s", "jian").size());

    // check
    EXPECT_TRUE(str::equal("chen", 4, "chen", 4));
    EXPECT_TRUE(str::prefix("chensoft", "chen"));
    EXPECT_FALSE(str::prefix("chensoft", "soft"));
    EXPECT_FALSE(str::prefix("chen", "chensoft"));
    EXPECT_TRUE(str::suffix("chensoft", "soft"));
    EXPECT_FALSE(str::suffix("chensoft", "chen"));
    EXPECT_FALSE(str::suffix("chen", "chensoft"));
    EXPECT_TRUE(str::contain("libchen", "lib"));

    // count
    EXPECT_EQ(3, str::count("I scream, you scream, we all scream for ice-cream!", "scream"));

    // split & join
    std::vector<std::string> split({"10", "0", "0", "1"});
    EXPECT_EQ(split, str::split("10.0.0.1", "."));
    EXPECT_EQ(split, str::split("10->0->0->1", "->"));
    EXPECT_EQ(std::vector<std::string>{"10"}, str::split("10", "."));
    EXPECT_EQ(std::vector<std::string>{}, str::split("", "."));
    EXPECT_EQ("10.0.0.1", str::join(split, "."));
    EXPECT_EQ("", str::join(std::vector<std::string>(), "."));

    // replace
    EXPECT_EQ("I'm a replace string", str::replace("I'm a search string", "search", "replace"));
    EXPECT_EQ("I'm a  string", str::remove("I'm a replace string", "replace"));

    EXPECT_EQ("I'm a replace string, and another search string", str::replace("I'm a search string, and another search string", "search", "replace", false));
    EXPECT_EQ("I'm a replace string, and another replace string", str::replace("I'm a search string, and another search string", "search", "replace"));

    // trim
    EXPECT_EQ("so many spaces  ", str::ltrim("  so many spaces  "));
    EXPECT_EQ("  so many spaces", str::rtrim("  so many spaces  "));
    EXPECT_EQ("so many spaces", str::trim("  so many spaces  "));

    // transform
    EXPECT_EQ("", str::capitalize(""));
    EXPECT_EQ("AbCDe", str::capitalize("abCDe"));
    EXPECT_EQ("abcde", str::lowercase("abCDe"));
    EXPECT_EQ("ABCDE", str::uppercase("abCDe"));

    // levenshtein
    EXPECT_EQ(2, str::levenshtein("pull", 4, "push", 4));
}