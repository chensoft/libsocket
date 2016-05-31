/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(BaseStrTest, General)
{
    // format
    EXPECT_EQ("", chen::str::format(""));
    EXPECT_EQ("plain", chen::str::format("plain"));
    EXPECT_EQ("year 2016", chen::str::format("year %d", 2016));

    // check
    EXPECT_TRUE(chen::str::equal("chen", 4, "chen", 4));
    EXPECT_TRUE(chen::str::hasPrefix("chensoft", "chen"));
    EXPECT_FALSE(chen::str::hasPrefix("chensoft", "soft"));
    EXPECT_TRUE(chen::str::hasSuffix("chensoft", "soft"));
    EXPECT_FALSE(chen::str::hasSuffix("chensoft", "chen"));
    EXPECT_TRUE(chen::str::contain("libchen", "lib"));

    // count
    EXPECT_EQ(3, chen::str::count("I scream, you scream, we all scream for ice-cream!", "scream"));

    // split & join
    std::vector<std::string> split({"10", "0", "0", "1"});
    EXPECT_EQ(split, chen::str::split("10.0.0.1", "."));
    EXPECT_EQ(split, chen::str::split("10->0->0->1", "->"));
    EXPECT_EQ("10.0.0.1", chen::str::join(split, "."));

    // replace
    std::string replace("I'm a search string");

    chen::str::replace(replace, "search", "replace");
    EXPECT_EQ("I'm a replace string", replace);

    chen::str::remove(replace, "replace");
    EXPECT_EQ("I'm a  string", replace);

    // trim
    std::string trim("  so many spaces  ");

    chen::str::ltrim(trim);
    EXPECT_EQ("so many spaces  ", trim);

    chen::str::rtrim(trim);
    EXPECT_EQ("so many spaces", trim);

    chen::str::trim(trim);
    EXPECT_EQ("so many spaces", trim);

    trim = "  so many spaces  ";
    chen::str::trim(trim);
    EXPECT_EQ("so many spaces", trim);
}