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

    client c = client::v4();

    PILogE("local: %s, remote: %s", c.local().str().c_str(), c.remote().str().c_str());
    PILogE("non-blocking: %d, valid: %d, native: %d", c.nonblocking(), c.valid(), c.native());
    PILogE("option: %d", c.option().nodelay());

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}