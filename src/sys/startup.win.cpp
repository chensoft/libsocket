/**
 * Created by Jian Chen
 * @since  2016.09.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/bsd/setup.hpp>
#include <socket/config.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// library
#pragma comment(lib, "ws2_32.lib")


// -----------------------------------------------------------------------------
// setup
chen::bsd::setup::setup()
{
    class helper
    {
    public:
        helper()
        {
            WORD version = MAKEWORD(2, 2);
            WSADATA data;

            auto ret = ::WSAStartup(version, &data);
            if (ret)
                throw std::system_error(sys::error(ret), "setup: failed to start winsock");

            if (data.wVersion != version)
                throw std::runtime_error("setup: request winsock version is wrong");
        }

        ~helper()
        {
            ::WSACleanup();
        }
    };

    static helper inst;
}

#endif