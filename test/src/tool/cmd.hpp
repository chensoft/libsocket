/**
 * Created by Jian Chen
 * @since  2016.06.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(ToolCmdTest, General)
{
    chen::cmd cmd;

    // help
    cmd.create("help", "show help");

    // version
    cmd.create("version", "show app version");

    // start
    cmd.create("start", "start server");
    cmd.define("port", "p", "server port (default: 80)", 80);
    cmd.define("addr", "a", "server address (default: 0.0.0.0)", "0.0.0.0");

    // simulate -> version
    std::vector<const char*> argv = {
            "app",
            "version"
    };

    cmd.parse(static_cast<int>(argv.size()), &argv[0]);

    EXPECT_EQ("version", cmd.current());

    // simulate -> start
    argv = {
            "app",
            "start"
    };

    cmd.parse(static_cast<int>(argv.size()), &argv[0]);

    EXPECT_EQ("start", cmd.current());
    EXPECT_EQ(80, cmd.intVal("port"));
    EXPECT_EQ("0.0.0.0", cmd.strVal("addr"));

    // simulate -> start
    argv = {
            "app",
            "start",
            "--port=8888",
            "--addr=127.0.0.1"
    };

    cmd.parse(static_cast<int>(argv.size()), &argv[0]);

    EXPECT_EQ("start", cmd.current());
    EXPECT_EQ(8888, cmd.intVal("port"));
    EXPECT_EQ("127.0.0.1", cmd.strVal("addr"));
}