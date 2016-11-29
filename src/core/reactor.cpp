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
void chen::reactor::run()
{
    this->run(1);
}

void chen::reactor::run(std::size_t count)
{
    while (true)
    {
        auto list = this->_backend.poll(count);
        if (list.empty())
            break;  // user request to stop or timeout

        for (auto &item : list)
        {
            switch (item.event)
            {
                case Event::Readable:
                    static_cast<reactor_delegate*>(item.data)->onReadable();
                    break;

                case Event::Writable:
                    static_cast<reactor_delegate*>(item.data)->onWritable();
                    break;

                case Event::Ended:
                    static_cast<reactor_delegate*>(item.data)->onEnded();
                    break;
            }
        }
    }
}

void chen::reactor::stop()
{
    this->_backend.stop();
}