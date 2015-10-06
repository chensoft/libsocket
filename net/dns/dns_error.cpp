/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_error.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// error
error::error(const std::string &what)
: std::runtime_error(what)
{

}


// -----------------------------------------------------------------------------
// error_fqdn
error_fqdn::error_fqdn(const std::string &what)
: error(what)
{

}


// -----------------------------------------------------------------------------
// error_size
error_size::error_size(const std::string &what)
: error(what)
{

}