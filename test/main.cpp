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
    using namespace chen::tcp;

//    client ccc = client::v4();
//
//    PILogE("local: %s, remote: %s", ccc.local().str().c_str(), ccc.remote().str().c_str());
//    PILogE("non-blocking: %d, valid: %d, native: %d", ccc.nonblocking(), ccc.valid(), ccc.native());
//    PILogE("option: %d", ccc.option().nodelay());
//
//    ccc.attach([] (client &c, connecting_event &e) {
//        PILogE("connecting: %s", e.ep.str().c_str());
//    });
//
//    ccc.connect("139.196.204.109:80");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}