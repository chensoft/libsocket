/**
 * Created by Jian Chen
 * @since  2016.05.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "callable.hpp"

using namespace chen;

// ------------------------------------------------------------------
// callable
callable::callable(callable &&o)
: _ptr(std::move(o._ptr))
{

}

callable& callable::operator=(callable &&o)
{
    if (this == &o)
        return *this;

    this->_ptr = std::move(o._ptr);

    return *this;
}

void callable::operator()()
{
    this->_ptr->call();
}