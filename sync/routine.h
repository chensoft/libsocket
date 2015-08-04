/**
 * Created by Jian Chen
 * @since  2015.08.04
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <functional>

// implement in future

namespace chen
{
    class routine final
    {
    public:
        static void async(const std::function<void ()> &job);

    private:
        routine() = default;
        ~routine() = default;

        routine(const routine&) = delete;
        routine& operator=(const routine&) = delete;
    };
}