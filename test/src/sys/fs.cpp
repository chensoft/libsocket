/**
 * Created by Jian Chen
 * @since  2016.06.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/fs.hpp>
#include <gtest/gtest.h>

TEST(SysFsTest, General)
{
    using chen::fs;

    // path
    fs::home();
    fs::temp();

    // drive
    EXPECT_EQ("C:\\", fs::drive("C:\\Windows\\System32"));
    EXPECT_EQ("/", fs::drive("/usr/local"));
    EXPECT_EQ("", fs::drive("file.txt"));
    EXPECT_EQ("", fs::drive(""));

    // path
    EXPECT_EQ(fs::current(), fs::realpath(fs::current()));
    EXPECT_EQ(fs::current(), fs::absolute(".", fs::current()));

    EXPECT_EQ(fs::home(), fs::realpath("~"));
    EXPECT_EQ(fs::home(), fs::absolute("~"));
    EXPECT_EQ(fs::home() + "/Downloads", fs::absolute("~/Downloads"));

    // normalize
    EXPECT_EQ("", fs::normalize(""));
    EXPECT_EQ("a", fs::normalize("./a"));
    EXPECT_EQ("a/b", fs::normalize("a/./b"));
    EXPECT_EQ("a/b", fs::normalize("a///b"));
    EXPECT_EQ("a/.../b", fs::normalize("a/.../b"));  // this is a invalid path
    EXPECT_EQ("../b", fs::normalize("a/../../b"));   // the second .. don't know how to removed
    EXPECT_EQ("/usr/local", fs::normalize("/usr/local/etc/.."));

    EXPECT_EQ("C:\\a", fs::normalize("C:\\a"));
    EXPECT_EQ("C:\\a", fs::normalize("C:\\.\\a"));
    EXPECT_EQ("C:\\a\\...\\b", fs::normalize("C:\\a\\...\\b"));
    EXPECT_EQ("C:\\..\\b", fs::normalize("C:\\a\\..\\..\\b"));
    EXPECT_EQ("C:\\b", fs::normalize("C:\\a\\..\\b"));

    // dirname
    EXPECT_EQ("", fs::dirname(""));
    EXPECT_EQ("/home/staff/Downloads", fs::dirname("/home/staff/Downloads/file.txt"));
    EXPECT_EQ("/usr", fs::dirname("/usr/."));
    EXPECT_EQ("/", fs::dirname("/usr/"));
    EXPECT_EQ("/", fs::dirname("/usr///"));
    EXPECT_EQ("/", fs::dirname("/"));
    EXPECT_EQ(".", fs::dirname("file.txt"));

    EXPECT_EQ("C:\\Windows", fs::dirname("C:\\Windows\\System32"));
    EXPECT_EQ("C:\\Windows\\System32", fs::dirname("C:\\Windows\\System32\\cmd.exe"));
    EXPECT_EQ("C:\\", fs::dirname("C:\\\\\\"));
    EXPECT_EQ("C:\\", fs::dirname("C:\\"));

    // basename
    EXPECT_EQ("file.txt", fs::basename("/home/staff/Downloads/file.txt"));
    EXPECT_EQ("home", fs::basename("/home/"));
    EXPECT_EQ("", fs::basename("/"));
    EXPECT_EQ("file.txt", fs::basename("file.txt"));
    EXPECT_EQ("file.txt", fs::basename("file.txt", "none"));
    EXPECT_EQ("file", fs::basename("file.txt", ".txt"));

    EXPECT_EQ("cmd.exe", fs::basename("C:\\Windows\\System32\\cmd.exe"));
    EXPECT_EQ("", fs::basename("C:\\"));

    // extname
    EXPECT_EQ("", fs::extname("file.txt", 0));
    EXPECT_EQ(".txt", fs::extname("/home/staff/Downloads/file.txt"));
    EXPECT_EQ("", fs::extname("/home/"));
    EXPECT_EQ("", fs::extname("/"));

    EXPECT_EQ(".exe", fs::extname("C:\\Windows\\System32\\cmd.exe"));
    EXPECT_EQ("", fs::extname("C:\\"));

    // absolute
    EXPECT_TRUE(fs::isAbsolute("/usr/local"));
    EXPECT_FALSE(fs::isAbsolute("file.txt"));
    EXPECT_FALSE(fs::isAbsolute(""));

    EXPECT_TRUE(fs::isAbsolute("C:\\Windows\\System32"));

    // change
    auto cwd = fs::current();
    fs::change(cwd);
    EXPECT_EQ(cwd, fs::current());

    // platform dependent
#ifndef _WIN32
    EXPECT_EQ("/", fs::root());
    EXPECT_EQ(std::vector<std::string>({"/"}), fs::drives());
    EXPECT_EQ('/', fs::separator());

    // absolute
    EXPECT_EQ(fs::normalize(fs::current() + "/../a/b"), fs::absolute("../a/b"));
    EXPECT_EQ("/a/b", fs::absolute("/a/b"));
    EXPECT_EQ("/a/b", fs::absolute("/a/b", fs::current()));

    // check
    EXPECT_TRUE(fs::isExist("/"));
    EXPECT_TRUE(fs::isDir("/"));
    EXPECT_FALSE(fs::isFile("/"));
    EXPECT_FALSE(fs::isLink("/"));

    // just call
    fs::isReadable("/");
    fs::isWritable("/");
    fs::isExecutable("/");

    fs::atime("/");
    fs::mtime("/");
    fs::ctime("/");

    // touch
    fs::write("/tmp/libchen_test.tmp", "content");
    fs::write("/tmp/libchen_test.tmp", "content", 7);
    fs::append("/tmp/libchen_test.tmp", "content");
    fs::append("/tmp/libchen_test.tmp", "content", 7);
    fs::read("/tmp/libchen_test.tmp", '\n');

    fs::touch("/tmp/libchen_test.tmp");
    fs::touch("/tmp/libchen_test.tmp", std::time(nullptr), std::time(nullptr));
    fs::copy("/tmp/libchen_test.tmp", "/tmp/libchen_test1.tmp");
    fs::rename("/tmp/libchen_test.tmp", "/tmp/libchen_test2.tmp");
    fs::remove("/tmp/libchen_test.tmp");
    fs::remove("/tmp/libchen_test1.tmp");
    fs::remove("/tmp/libchen_test2.tmp");

    fs::create("/tmp/folder_libchen/nest", 0, true);
    fs::copy("/tmp/folder_libchen", "/tmp/folder_libchen_copy");
    fs::remove("/tmp/folder_libchen");
    fs::remove("/tmp/folder_libchen_copy");

    // visit
    fs::collect("/", false);
    fs::count("/", false);
    fs::count("/", false, false, false);
    fs::visit("/", [] (const std::string &path) {
    }, false);
#endif
}