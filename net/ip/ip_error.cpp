/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "ip_error.h"

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// error
error::error(const std::string &what)
: std::runtime_error(what)
{

}


// -----------------------------------------------------------------------------
// error_convert
error_convert::error_convert(const std::string &what)
: error(what)
{

}


// -----------------------------------------------------------------------------
// error_invalid
error_invalid::error_invalid(const std::string &what)
: error(what)
{

}