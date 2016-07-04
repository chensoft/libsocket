/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/proc.hpp>
#include <chen/sys/fs.hpp>
#include <chen/base/str.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <cstdlib>

using namespace chen;

// -----------------------------------------------------------------------------
// proc
void proc::daemon()
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

std::string proc::path(int argc, const char *const argv[])
{
    if (!argc)
        return "";

    std::string name(argv[0]);

    // if argv[0] is absolute
    if (fs::isAbsolute(name))
        return name;

    // if argv[0] is an relative
    if (str::contain(name, "/"))
        return fs::current() + "/" + fs::normalize(name);

    // search $PATH for argv[0]
    std::string path(::getenv("PATH"));
    auto split = str::split(path, ":");

    for (auto &folder : split)
    {
        std::string full = folder + "/" + name;
        if (fs::isExecutable(full))
            return full;
    }

    return "";
}

int proc::pid()
{
    return ::getpid();
}

bool proc::kill(int pid)
{
    return !::kill(pid, SIGKILL);
}

bool proc::exist(int pid)
{
    return ::getpgid(pid) >= 0;
}