/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/chen.hpp"
#include "gtest/gtest.h"
#include "conf.hpp"

int main(int argc, char *argv[])
{
    chen::cmd cmd;
    cmd.define("data", "d", "test data path", "");
    cmd.define("gtest_filter", "", "dummy", "");
    cmd.define("gtest_color", "", "dummy", "");
    cmd.parse(argc, argv);

    conf::data = cmd.strVal("data");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}