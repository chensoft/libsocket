/**
 * Created by Jian Chen
 * @since  2016.11.24
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/core/reactor.hpp>

// -----------------------------------------------------------------------------
// reactor
chen::reactor::reactor()
{
}

chen::reactor::~reactor()
{
}

// modify
void chen::reactor::set(handle_t fd, reactor_delegate *delegate, int opcode, int flag)
{
    this->_backend.set(fd, delegate, opcode, flag);
}

void chen::reactor::del(handle_t fd)
{
    this->_backend.del(fd);
}

// control
void chen::reactor::run(std::size_t count, double timeout)
{
    while (true)
    {
        auto list = this->_backend.poll(count, timeout);
        if (list.empty())
            break;  // user request to stop or timeout

        for (auto &item : list)
        {
            switch (item.ev)
            {
                case Event::Readable:
                    static_cast<reactor_delegate*>(item.ptr)->onReadable();
                    break;

                case Event::Writable:
                    static_cast<reactor_delegate*>(item.ptr)->onWritable();
                    break;

                case Event::Ended:
                    static_cast<reactor_delegate*>(item.ptr)->onEnded();
                    break;
            }
        }
    }
}

void chen::reactor::stop()
{
    this->_backend.stop();
}