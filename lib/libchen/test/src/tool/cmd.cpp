/**
 * Created by Jian Chen
 * @since  2016.06.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/tool/cmd.hpp"
#include "gtest/gtest.h"

TEST(ToolCmdTest, General)
{
    chen::cmd cmd;

    // help
    cmd.create("help", "show help");
    cmd.create("help", "show app help");  // test overwrite
 
    // version
    cmd.create("version", "show app version");

    // sub-action
    cmd.create("module", "operate module");
    cmd.create("module.update", "update module info");

    // start
    cmd.create("start", "start server");
    cmd.define("port", "p", "server port (default: 80)", 80);
    cmd.define("addr", "a", "server address (default: 0.0.0.0)", "0.0.0.0");
    cmd.define("speed", "s", "speed limit", 1);
    cmd.define("thread", "t", "thread count (default: 1)", 1);
    cmd.define("daemon", "d", "run as a daemon (default: no)", false);

    EXPECT_THROW(cmd.define("", "", "", ""), chen::cmd::error_general);
    EXPECT_THROW(cmd.define("s", "", "", ""), chen::cmd::error_general);
    EXPECT_THROW(cmd.define("listen", "tiny", "", ""), chen::cmd::error_general);
    EXPECT_THROW(cmd.define("port", "", "", ""), chen::cmd::error_general);
    EXPECT_THROW(cmd.define("listen", "p", "", ""), chen::cmd::error_general);

    EXPECT_NO_THROW(cmd.change("start"));
    EXPECT_THROW(cmd.change("noaction"), chen::cmd::error_general);

    EXPECT_FALSE(cmd.exist("noaction"));
    EXPECT_FALSE(cmd.exist("start", "nooption"));

    // suggest
    cmd.suggest("edition", "version");

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
            "-a=127.0.0.1",
            "--thread",
            "4",
            "-s",
            "100",
            "127.0.0.1"
    };

    cmd.parse(static_cast<int>(argv.size()), &argv[0]);

    EXPECT_EQ("start", cmd.current());
    EXPECT_EQ(8888, cmd.intVal("port"));
    EXPECT_EQ("127.0.0.1", cmd.strVal("addr"));

    EXPECT_THROW(cmd.strVal("x"), chen::cmd::error);
    EXPECT_THROW(cmd.strVal("color"), chen::cmd::error);

    EXPECT_TRUE(cmd.isSet("port"));
    EXPECT_FALSE(cmd.objects().empty());

    // simulate -> sub-action
    argv = {
            "app",
            "module",
            "update",
    };

    cmd.parse(static_cast<int>(argv.size()), &argv[0]);

    EXPECT_EQ("module.update", cmd.current());

    // simulate -> exception
    argv = {
            "app",
            "start",
            "--a"  // it's not a long option name
    };

    EXPECT_THROW(cmd.parse(static_cast<int>(argv.size()), &argv[0]), chen::cmd::error);

    argv = {
            "app",
            "start",
            "--speedx"  // long option not exist
    };

    EXPECT_THROW(cmd.parse(static_cast<int>(argv.size()), &argv[0]), chen::cmd::error);

    argv = {
            "app",
            "start",
            "-x"  // short option not exist
    };

    EXPECT_THROW(cmd.parse(static_cast<int>(argv.size()), &argv[0]), chen::cmd::error);

    argv = {
            "app",
            "editi"
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