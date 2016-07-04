/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/proc.hpp>
#include <Windows.h>

using namespace chen;

// -----------------------------------------------------------------------------
// proc
std::string proc::path(int argc, const char *const argv[])
{
    CHAR buf[MAX_PATH] = { 0 };

    if (::GetModuleFileName(NULL, buf, sizeof(buf)))
        return std::string(buf);
    else
        return "";
}

int proc::pid()
{
    return ::GetCurrentProcessId();
}

bool proc::kill(int pid)
{
    HANDLE handle = ::OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (handle == INVALID_HANDLE_VALUE)
        return false;

    BOOL ret = ::TerminateProcess(handle, 0);

    ::CloseHandle(handle);
    return ret == TRUE;
}

bool proc::exist(int pid)
{
    HANDLE handle = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (handle == INVALID_HANDLE_VALUE)
        return false;

    DWORD status = 0;
    ::GetExitCodeProcess(handle, &status);

    ::CloseHandle(handle);
    return status == STILL_ACTIVE;
}