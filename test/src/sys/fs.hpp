/**
 * Created by Jian Chen
 * @since  2016.06.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(SysFsTest, General)
{
#ifndef _WIN32
    EXPECT_EQ("/", chen::fs::root());
    EXPECT_EQ(std::vector<std::string>({"/"}), chen::fs::drives());
    EXPECT_EQ('/', chen::fs::separator());

    EXPECT_EQ("a", chen::fs::normalize("./a"));
    EXPECT_EQ("a/b", chen::fs::normalize("a/./b"));
    EXPECT_EQ("a/b", chen::fs::normalize("a///b"));
    EXPECT_EQ("a/.../b", chen::fs::normalize("a/.../b"));  // this is a invalid path
    EXPECT_EQ("../b", chen::fs::normalize("a/../../b"));  // the second .. don't know how to removed

    EXPECT_EQ("/home/staff/Downloads", chen::fs::dirname("/home/staff/Downloads/file.txt"));
    EXPECT_EQ("/usr", chen::fs::dirname("/usr/."));
    EXPECT_EQ("/", chen::fs::dirname("/usr/"));
    EXPECT_EQ("/", chen::fs::dirname("/usr///"));
    EXPECT_EQ("/", chen::fs::dirname("/"));
    EXPECT_EQ(".", chen::fs::dirname("file.txt"));

    EXPECT_EQ("file.txt", chen::fs::basename("/home/staff/Downloads/file.txt"));
    EXPECT_EQ("file.txt", chen::fs::basename("file.txt"));

    EXPECT_EQ(".txt", chen::fs::extname("/home/staff/Downloads/file.txt"));
#endif
}