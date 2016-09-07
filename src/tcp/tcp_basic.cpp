/**
 * Created by Jian Chen
 * @since  2016.09.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_basic.hpp>

// -----------------------------------------------------------------------------
// basic
chen::tcp::basic::basic(socket &&sock) : socket(std::move(sock))
{
}