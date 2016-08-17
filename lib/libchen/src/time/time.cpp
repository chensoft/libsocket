/**
 * Created by Jian Chen
 * @since  2016.04.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/time/time.hpp>

// -----------------------------------------------------------------------------
// time
chen::time::time(bool start)
{
    if (start)
        this->start();
}

void chen::time::start()
{
    this->_beg = std::chrono::high_resolution_clock::now();
}

void chen::time::stop()
{
    this->_end = std::chrono::high_resolution_clock::now();
}

double chen::time::elapsed() const
{
    std::chrono::duration<double> off = this->_end - this->_beg;
    return off.count();
}