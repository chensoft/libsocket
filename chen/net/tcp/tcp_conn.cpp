/**
 * Created by Jian Chen
 * @since  2016.04.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "tcp_conn.hpp"

using namespace chen;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// conn
conn::conn(void *so)
: socket(so)
{

}

conn::~conn()
{

}