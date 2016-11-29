/**
 * Created by Jian Chen
 * @since  2016.11.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_socket.hpp>
#include <socket/base/basic_option.hpp>
#include <gtest/gtest.h>

using chen::basic_socket;
using chen::basic_option;
using chen::basic_address;

TEST(BasicOptionTest, General)
{
    basic_socket s(AF_INET, SOCK_STREAM, 0);
    basic_option o = s.option();


}