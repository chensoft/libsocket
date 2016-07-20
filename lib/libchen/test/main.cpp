/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "conf.hpp"

#include "src/base/any.hpp"
#include "src/base/map.hpp"
#include "src/base/num.hpp"
#include "src/base/str.hpp"
#include "src/base/utf8.hpp"
#include "src/base/regex.hpp"
#include "src/base/vector.hpp"

#include "src/data/ini.hpp"
#include "src/data/json.hpp"

#include "src/mt/threadpool.hpp"

#include "src/sys/fs.hpp"
#include "src/sys/sys.hpp"

#include "src/tool/cmd.hpp"

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