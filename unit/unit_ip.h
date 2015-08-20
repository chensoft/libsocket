/**
 * Created by Jian Chen
 * @since  2015.08.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "unit_base.h"
#include "net/ip/ip_addr.h"

namespace chen
{
    class unit_ip : public unit_base
    {
    public:
        virtual void check() override
        {
            // test address assign
            this->check_address_assign();

            // test address class
            this->check_address_class();

            // test address property
            this->check_address_property();
        }

        void check_address_assign()
        {
            PILogD("unit ip: test address assign start...");

            chen::ip::address_v4 v4;

            // empty: 0.0.0.0
            v4.assign("");
            this->assert(v4.str() == "0.0.0.0", "unit ip: empty -> 0.0.0.0, but addr is %s", v4.str().c_str());

            // one: 127 -> 127.0.0.0
            v4.assign("127");
            this->assert(v4.str() == "127.0.0.0", "unit ip: 127 -> 127.0.0.0, but addr is %s", v4.str().c_str());

            // two: 127.1 -> 127.0.0.1
            v4.assign("127.1");
            this->assert(v4.str() == "127.0.0.1", "unit ip: 127.1 -> 127.0.0.1, but addr is %s", v4.str().c_str());

            // three: 1.2.3 -> 1.2.0.3
            v4.assign("1.2.3");
            this->assert(v4.str() == "1.2.0.3", "unit ip: 1.2.3 -> 1.2.0.3, but addr is %s", v4.str().c_str());

            // four: 192.168.10.1
            v4.assign("192.168.10.1");
            this->assert(v4.str() == "192.168.10.1", "unit ip: 192.168.10.1 -> 192.168.10.1, but addr is %s", v4.str().c_str());

            PILogD("unit ip: test address assign success\n");
        }

        void check_address_class()
        {
            PILogD("unit ip: test address class start...");

            chen::ip::address_v4 v4;

            // class A
            v4.assign("0.0.0.0");
            this->assert(v4.is_class_a(), "unit ip: 0.0.0.0 is class A");
            this->assert(!v4.is_class_b(), "unit ip: 0.0.0.0 isn't class B");
            this->assert(!v4.is_class_c(), "unit ip: 0.0.0.0 isn't class C");

            v4.assign("10.0.0.0");
            this->assert(v4.is_class_a(), "unit ip: 10.0.0.0 is class A");
            this->assert(!v4.is_class_b(), "unit ip: 10.0.0.0 isn't class B");
            this->assert(!v4.is_class_c(), "unit ip: 10.0.0.0 isn't class C");

            v4.assign("127.255.255.255");
            this->assert(v4.is_class_a(), "unit ip: 127.255.255.255 is class A");
            this->assert(!v4.is_class_b(), "unit ip: 127.255.255.255 isn't class B");
            this->assert(!v4.is_class_c(), "unit ip: 127.255.255.255 isn't class C");

            // class B
            v4.assign("128.0.0.0");
            this->assert(!v4.is_class_a(), "unit ip: 128.0.0.0 isn't class A");
            this->assert(v4.is_class_b(), "unit ip: 128.0.0.0 is class B");
            this->assert(!v4.is_class_c(), "unit ip: 128.0.0.0 isn't class C");

            v4.assign("190.0.0.0");
            this->assert(!v4.is_class_a(), "unit ip: 190.0.0.0 isn't class A");
            this->assert(v4.is_class_b(), "unit ip: 190.0.0.0 is class B");
            this->assert(!v4.is_class_c(), "unit ip: 190.0.0.0 isn't class C");

            v4.assign("191.255.255.255");
            this->assert(!v4.is_class_a(), "unit ip: 191.255.255.255 isn't class A");
            this->assert(v4.is_class_b(), "unit ip: 191.255.255.255 is class B");
            this->assert(!v4.is_class_c(), "unit ip: 191.255.255.255 isn't class C");

            // class C
            v4.assign("192.0.0.0");
            this->assert(!v4.is_class_a(), "unit ip: 192.0.0.0 isn't class A");
            this->assert(!v4.is_class_b(), "unit ip: 192.0.0.0 isn't class B");
            this->assert(v4.is_class_c(), "unit ip: 192.0.0.0 is class C");

            v4.assign("192.168.1.1");
            this->assert(!v4.is_class_a(), "unit ip: 192.168.1.1 isn't class A");
            this->assert(!v4.is_class_b(), "unit ip: 192.168.1.1 isn't class B");
            this->assert(v4.is_class_c(), "unit ip: 192.168.1.1 is class C");

            v4.assign("223.255.255.255");
            this->assert(!v4.is_class_a(), "unit ip: 223.255.255.255 isn't class A");
            this->assert(!v4.is_class_b(), "unit ip: 223.255.255.255 isn't class B");
            this->assert(v4.is_class_c(), "unit ip: 223.255.255.255 is class C");

            PILogD("unit ip: test address class success\n");
        }

        void check_address_property()
        {
            PILogD("unit ip: test address property start...");

            chen::ip::address_v4 v4;

            // empty
            v4.assign("");
            this->assert(v4.empty(), "unit ip: addr should be empty, but addr is %u", v4.addr());

            // loopback
            v4.assign("127.0.0.1");
            this->assert(v4.is_loopback(), "unit ip: 127.0.0.1 should be loopback");

            v4.assign("192.168.0.1");
            this->assert(!v4.is_loopback(), "unit ip: 192.168.0.1 should not be loopback");

            // broadcast
            v4.assign("192.168.0.255", 24);
            this->assert(v4.is_broadcast(), "unit ip: 192.168.0.255 with 0xFFFFFF00 is broadcast");

            v4.assign("192.168.0.1", 24);
            this->assert(!v4.is_broadcast(), "unit ip: 192.168.0.1 with 0xFFFFFF00 isn't broadcast");

            // multicast
            v4.assign("224.0.0.115");
            this->assert(v4.is_multicast(), "unit ip: 224.0.0.115 is multicast");

            v4.assign("10.0.0.1");
            this->assert(!v4.is_multicast(), "unit ip: 10.0.0.1 isn't multicast");

            // network
            v4.assign("192.168.1.10");

            this->assert(v4.network().str() == "192.168.1.0", "unit ip: 192.168.1.10 network address is 192.168.1.0");
            this->assert(v4.host_min().str() == "192.168.1.1", "unit ip: 192.168.1.10 network's min host is 192.168.1.1");
            this->assert(v4.host_max().str() == "192.168.1.254", "unit ip: 192.168.1.10 network's min host is 192.168.1.254");

            // broadcast
            v4.assign("192.168.1.1", 30);
            this->assert(v4.broadcast().str() == "192.168.1.3", "unit ip: 192.168.1.1's broadcast should be 192.168.1.3");

            PILogD("unit ip: test address property success\n");
        }

        virtual void bench() override
        {

        }
    };
}