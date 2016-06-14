/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _MSC_VER
#pragma warning(disable:4819)
#endif

#include "src/base/any.hpp"
#include "src/base/map.hpp"
#include "src/base/num.hpp"
#include "src/base/regex.hpp"
#include "src/base/str.hpp"
#include "src/base/utf8.hpp"
#include "src/base/vector.hpp"

#include "src/sys/fs.hpp"
#include "src/sys/sys.hpp"

#include "src/tool/cmd.hpp"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}