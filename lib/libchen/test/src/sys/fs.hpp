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
    // path
    chen::fs::home();
    chen::fs::temp();

    // drive
    EXPECT_EQ("C:\\", chen::fs::drive("C:\\Windows\\System32"));
    EXPECT_EQ("/", chen::fs::drive("/usr/local"));
    EXPECT_EQ("", chen::fs::drive("file.txt"));
    EXPECT_EQ("", chen::fs::drive(""));

    // path
    EXPECT_EQ(chen::fs::current(), chen::fs::realpath(chen::fs::current()));
    EXPECT_EQ(chen::fs::current(), chen::fs::absolute(".", chen::fs::current()));

    // normalize
    EXPECT_EQ("", chen::fs::normalize(""));
    EXPECT_EQ("a", chen::fs::normalize("./a"));
    EXPECT_EQ("a/b", chen::fs::normalize("a/./b"));
    EXPECT_EQ("a/b", chen::fs::normalize("a///b"));
    EXPECT_EQ("a/.../b", chen::fs::normalize("a/.../b"));  // this is a invalid path
    EXPECT_EQ("../b", chen::fs::normalize("a/../../b"));   // the second .. don't know how to removed
    EXPECT_EQ("/usr/local", chen::fs::normalize("/usr/local/etc/.."));

    EXPECT_EQ("C:\\a", chen::fs::normalize("C:\\a"));
    EXPECT_EQ("C:\\a", chen::fs::normalize("C:\\.\\a"));
    EXPECT_EQ("C:\\a\\...\\b", chen::fs::normalize("C:\\a\\...\\b"));
    EXPECT_EQ("C:\\..\\b", chen::fs::normalize("C:\\a\\..\\..\\b"));
    EXPECT_EQ("C:\\b", chen::fs::normalize("C:\\a\\..\\b"));

    // dirname
    EXPECT_EQ("", chen::fs::dirname(""));
    EXPECT_EQ("/home/staff/Downloads", chen::fs::dirname("/home/staff/Downloads/file.txt"));
    EXPECT_EQ("/usr", chen::fs::dirname("/usr/."));
    EXPECT_EQ("/", chen::fs::dirname("/usr/"));
    EXPECT_EQ("/", chen::fs::dirname("/usr///"));
    EXPECT_EQ("/", chen::fs::dirname("/"));
    EXPECT_EQ(".", chen::fs::dirname("file.txt"));

    EXPECT_EQ("C:\\Windows", chen::fs::dirname("C:\\Windows\\System32"));
    EXPECT_EQ("C:\\Windows\\System32", chen::fs::dirname("C:\\Windows\\System32\\cmd.exe"));
    EXPECT_EQ("C:\\", chen::fs::dirname("C:\\\\\\"));
    EXPECT_EQ("C:\\", chen::fs::dirname("C:\\"));

    // basename
    EXPECT_EQ("file.txt", chen::fs::basename("/home/staff/Downloads/file.txt"));
    EXPECT_EQ("home", chen::fs::basename("/home/"));
    EXPECT_EQ("", chen::fs::basename("/"));
    EXPECT_EQ("file.txt", chen::fs::basename("file.txt"));

    EXPECT_EQ("cmd.exe", chen::fs::basename("C:\\Windows\\System32\\cmd.exe"));
    EXPECT_EQ("", chen::fs::basename("C:\\"));

    // extname
    EXPECT_EQ("", chen::fs::extname("file.txt", 0));
    EXPECT_EQ(".txt", chen::fs::extname("/home/staff/Downloads/file.txt"));
    EXPECT_EQ("", chen::fs::extname("/home/"));
    EXPECT_EQ("", chen::fs::extname("/"));

    EXPECT_EQ(".exe", chen::fs::extname("C:\\Windows\\System32\\cmd.exe"));
    EXPECT_EQ("", chen::fs::extname("C:\\"));

    // absolute
    EXPECT_TRUE(chen::fs::isAbsolute("/usr/local"));
    EXPECT_FALSE(chen::fs::isAbsolute("file.txt"));
    EXPECT_FALSE(chen::fs::isAbsolute(""));

    EXPECT_TRUE(chen::fs::isAbsolute("C:\\Windows\\System32"));

    // change
    auto cwd = chen::fs::current();
    chen::fs::change(cwd);
    EXPECT_EQ(cwd, chen::fs::current());

    // platform dependent
#ifndef _WIN32
    EXPECT_EQ("/", chen::fs::root());
    EXPECT_EQ(std::vector<std::string>({"/"}), chen::fs::drives());
    EXPECT_EQ('/', chen::fs::separator());

    // absolute
    EXPECT_EQ(chen::fs::normalize(chen::fs::current() + "/../a/b"), chen::fs::absolute("../a/b"));
    EXPECT_EQ("/a/b", chen::fs::absolute("/a/b"));
    EXPECT_EQ("/a/b", chen::fs::absolute("/a/b", chen::fs::current()));

    // check
    EXPECT_TRUE(chen::fs::isExist("/"));
    EXPECT_TRUE(chen::fs::isDir("/"));
    EXPECT_FALSE(chen::fs::isFile("/"));
    EXPECT_FALSE(chen::fs::isLink("/"));

    // just call
    chen::fs::isReadable("/");
    chen::fs::isWritable("/");
    chen::fs::isExecutable("/");

    chen::fs::atime("/");
    chen::fs::mtime("/");
    chen::fs::ctime("/");

    // touch
    chen::fs::write("/tmp/test_libchen.tmp", "content", false);
    chen::fs::write("/tmp/test_libchen.tmp", "content", 7, true);
    chen::fs::read("/tmp/test_libchen.tmp", '\n');

    chen::fs::touch("/tmp/test_libchen.tmp");
    chen::fs::touch("/tmp/test_libchen.tmp", std::time(nullptr), std::time(nullptr));
    chen::fs::copy("/tmp/test_libchen.tmp", "/tmp/test_libchen1.tmp");
    chen::fs::rename("/tmp/test_libchen.tmp", "/tmp/test_libchen2.tmp");
    chen::fs::remove("/tmp/test_libchen.tmp");
    chen::fs::remove("/tmp/test_libchen1.tmp");
    chen::fs::remove("/tmp/test_libchen2.tmp");

    chen::fs::create("/tmp/folder_libchen/nest", 0, true);
    chen::fs::copy("/tmp/folder_libchen", "/tmp/folder_libchen_copy");
    chen::fs::remove("/tmp/folder_libchen");
    chen::fs::remove("/tmp/folder_libchen_copy");

    // visit
    chen::fs::collect("/", false);
    chen::fs::count("/", false);
    chen::fs::count("/", false, false, false);
    chen::fs::visit("/", [] (const std::string &path) -> bool {
        return true;
    }, false);
#endif
}