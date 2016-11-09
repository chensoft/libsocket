/**
 * Created by Jian Chen
 * @since  2016.09.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/core/startup.hpp>
#include <socket/config.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// library
#pragma comment(lib, "ws2_32.lib")


// -----------------------------------------------------------------------------
// startup
chen::startup::startup()
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
                throw std::system_error(sys::error(ret), "startup: failed to start winsock");

            if (data.wVersion != version)
                throw std::runtime_error("startup: request winsock version is incorrect");
        }

        ~helper()
        {
            ::WSACleanup();
        }
    };

    static helper dummy;
}

#endif