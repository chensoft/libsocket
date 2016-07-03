/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/sys.hpp>
#include <Windows.h>

using namespace chen;

// -----------------------------------------------------------------------------
// sys
std::string sys::error()
{
    char buf[1024] = {0};
    return errno && !::strerror_s(buf, sizeof(buf), errno) ? std::string(buf) : "No error";
}

std::string sys::exe(int argc, const char *const argv[])
{
	CHAR buf[MAX_PATH] = { 0 };

	if (::GetModuleFileName(NULL, buf, sizeof(buf)))
		return std::string(buf);
	else
		return "";
}