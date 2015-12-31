/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "test.hpp"
#include "test_cmd.hpp"
#include "test_ip.hpp"
#include "test_udp.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// test
test& test::instance()
{
    static test inst;
    return inst;
}

test::test()
{
    // register all unit test
    this->_store["cmd"] = std::unique_ptr<test_base>(new test_cmd);
    this->_store["ip"]  = std::unique_ptr<test_base>(new test_ip);
    this->_store["udp"] = std::unique_ptr<test_base>(new test_udp);
}

void test::check()
{
    for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
        it->second->check();
}

void test::bench()
{
    for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
        it->second->bench();
}