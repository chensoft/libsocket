/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/socket.hpp>
#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    using namespace chen;

    bsd::socket s(0);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}