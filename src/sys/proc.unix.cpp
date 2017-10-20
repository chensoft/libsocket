/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include "chen/sys/proc.hpp"
#include "chen/sys/fs.hpp"
#include "chen/base/str.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <cstdlib>

// -----------------------------------------------------------------------------
// proc
bool chen::proc::daemon()
{
    // Note:
    // code is copied from http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
    // thanks to its author forking the parent process
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

    // Note:
    // http://www.microhowto.info/howto/cause_a_process_to_become_a_daemon_in_c.html
    // closing standard file descriptors and avoid these file descriptors to be reused
    ::close(STDIN_FILENO);
    ::close(STDOUT_FILENO);
    ::close(STDERR_FILENO);

    if (::open("/dev/null", O_RDONLY) < 0)
        std::exit(EXIT_FAILURE);

    if (::open("/dev/null", O_WRONLY) < 0)
        std::exit(EXIT_FAILURE);

    if (::open("/dev/null", O_RDWR) < 0)
        std::exit(EXIT_FAILURE);

    return true;
}

std::string chen::proc::exec(const std::string &command)
{
    FILE *pipe = ::popen(command.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("proc: open command failed");

    char buffer[128]{};
    std::string result;

    while (!::feof(pipe) && (::fgets(buffer, 128, pipe) != nullptr))
        result += buffer;

    ::pclose(pipe);

    return result;
}

std::string chen::proc::path(int argc, const char *const argv[])
{
    if (!argc)
        return "";

    std::string name(argv[0]);

    // if argv[0] is absolute
    if (fs::isAbsolute(name))
        return name;

    // if argv[0] is an relative
    if (str::contain(name, "/"))
        return fs::normalize(fs::current() + "/" + name);

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

int chen::proc::pid()
{
    return ::getpid();
}

bool chen::proc::kill(int pid)
{
    return !::kill(pid, SIGKILL);
}

bool chen::proc::exist(int pid)
{
    return ::getpgid(pid) >= 0;
}

#endif