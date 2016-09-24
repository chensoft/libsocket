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
//    try
//    {
//        chen::tcp::server s(chen::ip::address::Type::IPv4);
//        s.start("127.0.0.1:8888");
//    }
//    catch (const std::system_error &e)
//    {
//        PILogE("haha: %s", e.what());
//    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}