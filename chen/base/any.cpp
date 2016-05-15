/**
 * any
 * @since  2016.01.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "any.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// any
any::any(const any &o)
: _ptr(o._ptr ? o._ptr->clone() : nullptr)
{
}

any::any(any &&o)
{
    *this = std::move(o);
}

// copy & move
any& any::operator=(const any &o)
{
    if (this->_ptr == o._ptr)
        return *this;

    if (o._ptr)
        this->_ptr.reset(o._ptr->clone());
    else
        this->clear();

    return *this;
}

any& any::operator=(any &&o)
{
    if (this->_ptr == o._ptr)
        return *this;

    this->_ptr = std::move(o._ptr);

    return *this;
}

// empty
bool any::empty() const
{
    return this->_ptr == nullptr;
}

// clear
void any::clear()
{
    this->_ptr.reset();
}