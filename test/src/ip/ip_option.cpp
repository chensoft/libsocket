/**
 * Created by Jian Chen
 * @since  2017.02.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "socket/base/basic_socket.hpp"
#include "gtest/gtest.h"

#ifdef _WIN32
#include <ws2tcpip.h>
#endif

using chen::ip_option4;
using chen::ip_option6;
using chen::basic_socket;

TEST(IPOptionTest, General)
{
    basic_socket s4(AF_INET, SOCK_STREAM);
    basic_socket s6(AF_INET6, SOCK_STREAM);

    // ttl
    EXPECT_GE(ip_option4::ttl(s4.native()), 0);
    EXPECT_TRUE(ip_option4::ttl(s4.native(), 10));

    // v6only
    EXPECT_NO_THROW(ip_option6::v6only(s6.native()));  // default is true on Windows, otherwise false
    EXPECT_TRUE(ip_option6::v6only(s6.native(), true));
}