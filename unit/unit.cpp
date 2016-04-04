/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "unit.hpp"
#include "unit_cmd.hpp"
#include "unit_ip.hpp"
#include "unit_udp.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// test
unit& unit::instance()
{
    static unit inst;
    return inst;
}

unit::unit()
{
    // todo
    // register all unit test
//    this->_store["cmd"] = std::unique_ptr<unit_base>(new unit_cmd);
    this->_store["ip"]  = std::unique_ptr<unit_base>(new unit_ip);
    this->_store["udp"] = std::unique_ptr<unit_base>(new unit_udp);
}

void unit::check()
{
    for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
        it->second->check();
}

void unit::bench()
{
    for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
        it->second->bench();
}