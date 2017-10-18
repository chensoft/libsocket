/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include "chen/sys/proc.hpp"
#include <Windows.h>

// -----------------------------------------------------------------------------
// proc
bool chen::proc::daemon()
{
    return false;
}

std::string chen::proc::exec(const std::string &command)
{
    FILE *pipe = ::_popen(command.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("proc: open command failed");

    char buffer[128]{};
    std::string result;

    while (!::feof(pipe) && (::fgets(buffer, 128, pipe) != nullptr))
        result += buffer;

    ::_pclose(pipe);

    return result;
}

std::string chen::proc::path(int argc, const char *const argv[])
{
    CHAR buf[MAX_PATH] = { 0 };

    if (::GetModuleFileName(NULL, buf, sizeof(buf)))
        return std::string(buf);
    else
        return "";
}

int chen::proc::pid()
{
    return ::GetCurrentProcessId();
}

bool chen::proc::kill(int pid)
{
    HANDLE handle = ::OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (handle == INVALID_HANDLE_VALUE)
        return false;

    BOOL ret = ::TerminateProcess(handle, 0);

    ::CloseHandle(handle);
    return ret == TRUE;
}

bool chen::proc::exist(int pid)
{
    HANDLE handle = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (handle == INVALID_HANDLE_VALUE)
        return false;

    DWORD status = 0;
    ::GetExitCodeProcess(handle, &status);

    ::CloseHandle(handle);
    return status == STILL_ACTIVE;
}

#endif