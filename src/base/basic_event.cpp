/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_event.hpp>

// -----------------------------------------------------------------------------
// basic_event
const int chen::basic_event::Readable = 1 << 0;
const int chen::basic_event::Writable = 1 << 1;
const int chen::basic_event::Closed   = 1 << 2;