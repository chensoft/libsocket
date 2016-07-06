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
    cmd.create("help", "show app help");  // test overwrite
 
    // version
    cmd.create("version", "show app version");

    // start
    cmd.create("start", "start server");
    cmd.define("port", "p", "server port (default: 80)", 80);
    cmd.define("addr", "a", "server address (default: 0.0.0.0)", "0.0.0.0");
    cmd.define("daemon", "d", "run as a daemon (default: no)", false);

    EXPECT_THROW(cmd.define("", "", "", ""), chen::cmd::error_general);
    EXPECT_THROW(cmd.define("s", "", "", ""), chen::cmd::error_general);
    EXPECT_THROW(cmd.define("listen", "tiny", "", ""), chen::cmd::error_general);
    EXPECT_THROW(cmd.define("port", "", "", ""), chen::cmd::error_general);
    EXPECT_THROW(cmd.define("listen", "p", "", ""), chen::cmd::error_general);

    cmd.change("start");  // just test

    EXPECT_FALSE(cmd.exist("noaction"));
    EXPECT_FALSE(cmd.exist("start", "nooption"));

    // simulate -> version
    std::vector<const char*> argv = {
            "app",
            "version"
    };

    cmd.parse(static_cast<int>(argv.size()), &argv[0]);

    EXPECT_EQ("app", cmd.app());
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

    EXPECT_FALSE(cmd.isSet("daemon"));
    EXPECT_FALSE(cmd.boolVal("daemon"));

    // simulate -> start
    argv = {
            "app",
            "start",
            "--port=8888",
            "-a=127.0.0.1"
    };

    cmd.parse(static_cast<int>(argv.size()), &argv[0]);

    EXPECT_EQ("start", cmd.current());
    EXPECT_EQ(8888, cmd.intVal("port"));
    EXPECT_EQ("127.0.0.1", cmd.strVal("addr"));

    EXPECT_TRUE(cmd.isSet("port"));
    EXPECT_TRUE(cmd.objects().empty());

    // simulate -> exception
    argv = {
            "app",
            "ver"
    };

    try
    {
        cmd.parse(static_cast<int>(argv.size()), &argv[0]);
    }
    catch (const chen::cmd::error_parse &error)
    {
        cmd.usage(error);
    }

    // just call
    cmd.usage();
    cmd.usage("start");
    cmd.usage("start", "");
    cmd.usage("start", "port");

    cmd.visit([] (const chen::cmd::action &action, std::size_t idx, std::size_t len) {
    });

    cmd.visit("start", [] (const chen::cmd::option &option, std::size_t idx, std::size_t len) {
    });
}