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
: _ptr(o._ptr)
{
    o._ptr = nullptr;
}

any::~any()
{
    this->clear();
}

// copy & move
any& any::operator=(const any &o)
{
    if (this->_ptr == o._ptr)
        return *this;

    this->clear();

    if (o._ptr)
        this->_ptr = o._ptr->clone();

    return *this;
}

any& any::operator=(any &&o)
{
    if (this->_ptr == o._ptr)
        return *this;

    this->clear();

    this->_ptr = o._ptr;
    o._ptr     = nullptr;

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
    delete this->_ptr;
    this->_ptr = nullptr;
}