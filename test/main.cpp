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
    chen::bsd::socket s;
    s.nonblocking();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}