/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/ev_base.hpp>

// -----------------------------------------------------------------------------
// ev_base
void chen::ev_base::onAttach(reactor *loop, int mode, int flag)
{
    this->_ev_loop = loop;
    this->_ev_mode = mode;
    this->_ev_flag = flag;
}

void chen::ev_base::onDetach()
{
    this->_ev_loop = nullptr;
    this->_ev_mode = 0;
    this->_ev_flag = 0;
}