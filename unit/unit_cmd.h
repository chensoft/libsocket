/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "unit_base.h"
#include "tool/cmd.h"

namespace chen
{
    class unit_cmd : public unit_base
    {
    public:
        virtual void check() override
        {
            chen::cmd flag;


        }

        virtual void bench() override
        {

        }
    };
}