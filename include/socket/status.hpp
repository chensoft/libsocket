/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

// todo implement ourself error_code
#include <system_error>

namespace chen
{
    class status : public std::error_code
    {
    public:
        status() = default;
        status(const std::error_code &code) : error_code(code.value(), code.category()) {}
        status(int val, const std::error_category &cat) : error_code(val, cat) {}

    public:
        /**
         * Check error code is EWOULDBLOCK or EAGAIN
         */
        bool pending() const
        {
            return (*this == std::errc::operation_would_block) || (*this == std::errc::resource_unavailable_try_again);
        }
    };
}