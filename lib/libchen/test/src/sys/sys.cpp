/**
 * Created by Jian Chen
 * @since  2016.06.06
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/sys.hpp>
#include <gtest/gtest.h>

using namespace chen;

TEST(SysSysTest, General)
{
    EXPECT_NE(sys::uuid(), sys::uuid());
}