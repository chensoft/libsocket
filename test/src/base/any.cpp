/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/any.hpp>
#include <gtest/gtest.h>

TEST(BaseAnyTest, General)
{
    using chen::any;

    // empty
    any val;

    EXPECT_TRUE(val.empty());

    // non-empty
    val = 15;

    EXPECT_FALSE(val.empty());

    // integer
    int a = val;

    EXPECT_NE(0, a);
    EXPECT_NE(10, a);

    EXPECT_EQ(15, a);

    // string
    val = std::string("chensoft.com");
    std::string s = val;

    EXPECT_NE(std::string("chensoft"), s);
    EXPECT_EQ(std::string("chensoft.com"), s);

    // assign
    any o1(std::move(val));

    EXPECT_TRUE(val.empty());
    EXPECT_FALSE(o1.empty());

    any o2;
    o2 = std::move(o1);

    EXPECT_TRUE(o1.empty());
    EXPECT_FALSE(o2.empty());

    any o3;

    o3 = static_cast<const any&>(o3);  // self assign
    EXPECT_TRUE(o3.empty());

    o3 = 15;
    o3 = static_cast<const any&>(o1);  // test assign empty object
    EXPECT_TRUE(o3.empty());

    o3 = std::move(o1);  // self rvalue assign
    EXPECT_TRUE(o3.empty());
}