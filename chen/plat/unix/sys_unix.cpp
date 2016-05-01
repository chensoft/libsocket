/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include <chen/tool/sys.hpp>
#include <sys/stat.h>
#include <execinfo.h>
#include <unistd.h>
#include <cstdlib>

using namespace chen;

// -----------------------------------------------------------------------------
// sys
void sys::daemon()
{
    // Note:
    // code is copied from http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html, thanks to its author
    // forking the parent process
    pid_t pid = ::fork();

    // pid less than zero means error
    // zero means it's the child process
    // greater than zero means the parent process
    if (pid < 0)
        std::exit(EXIT_FAILURE);
    else if (pid > 0)
        std::exit(EXIT_SUCCESS);

    // changing the file mode mask(umask)
    ::umask(0);

    // creating a unique session ID(SID)
    pid_t sid = ::setsid();
    if (sid < 0)
        std::exit(EXIT_FAILURE);

    // changing the working directory
    if (::chdir("/") < 0)
        std::exit(EXIT_FAILURE);

    // closing standard file descriptors
    ::close(STDIN_FILENO);
    ::close(STDOUT_FILENO);
    ::close(STDERR_FILENO);
}

std::vector<std::string> sys::stack()
{
    void *buffer[1024];

    auto size = ::backtrace(buffer, 1024);
    auto list = ::backtrace_symbols(buffer, size);

    if (!list)
        return {};

    std::vector<std::string> ret;

    for (int i = 0; i < size; ++i)
        ret.push_back(list[i]);

    ::free(list);

    return ret;
}

#endif  // CHEN_OS_UNIX