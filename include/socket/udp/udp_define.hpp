/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstddef>

namespace chen
{
    constexpr std::size_t UDP_MAX_PACKET = 65507;  // 65535 − 8 byte UDP header − 20 byte IP header = 65507
}