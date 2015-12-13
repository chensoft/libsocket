/**
 * Created by Jian Chen
 * @since  2015.12.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_record.h"
#include "dns_codec.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// A

std::vector<std::uint8_t> A::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->address, store);
    return std::move(store);
}