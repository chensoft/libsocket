/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "semaphore.hpp"

using namespace chen;

// ------------------------------------------------------------------
// semaphore
void semaphore::wake()
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    ++this->_count;
    this->_cond.notify_one();
}

void semaphore::wait()
{
    std::unique_lock<std::mutex> lock(this->_mutex);

    while (!this->_count)
        this->_cond.wait(lock);

    --this->_count;
}