/**
 * Created by Jian Chen
 * @since  2016.04.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/fs.hpp>
#include <chen/base/str.hpp>
#include <fstream>
#include <cstdlib>
#include <climits>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <utime.h>
#include <pwd.h>

using namespace chen;

// -----------------------------------------------------------------------------
// path
std::string fs::root()
{
    return "/";
}

std::string fs::home()
{
    // check env variable first
    auto env = ::getenv("HOME");
    if (env)
        return env;

    // check login info
    struct passwd *pw = ::getpwuid(::getuid());
    return pw->pw_dir;
}

std::string fs::temp()
{
    auto env = ::getenv("TMPDIR");

    if (env)
    {
        // remove the trailing slash
        auto len = ::strlen(env);

        if (env[len - 1] == '/')
            return std::string(env, len - 1);
        else
            return env;
    }
    else
    {
        return "/tmp";
    }
}

std::string fs::current()
{
    char cwd[PATH_MAX] = {0};
    ::getcwd(cwd, PATH_MAX);

    return std::string(cwd);
}

std::vector<std::string> fs::drives()
{
    return {"/"};
}

char fs::separator()
{
    return '/';
}

std::string fs::realpath(const std::string &path)
{
    char buf[PATH_MAX] = {0};
    ::realpath(path.c_str(), buf);

    std::string ret(buf);
    return fs::isExist(ret) ? ret : "";
}

// exist
bool fs::isExist(const std::string &path)
{
    return !::access(path.c_str(), F_OK);
}

bool fs::isDir(const std::string &path, bool strict)
{
    struct stat st = {0};
    auto ok = strict ? !::lstat(path.c_str(), &st) : !::stat(path.c_str(), &st);
    return ok && S_ISDIR(st.st_mode);
}

bool fs::isFile(const std::string &path, bool strict)
{
    struct stat st = {0};
    auto ok = strict ? !::lstat(path.c_str(), &st) : !::stat(path.c_str(), &st);
    return ok && S_ISREG(st.st_mode);
}

bool fs::isLink(const std::string &path)
{
    struct stat st = {0};
    return !::lstat(path.c_str(), &st) && S_ISLNK(st.st_mode);
}

bool fs::isReadable(const std::string &path)
{
    return !::access(path.c_str(), R_OK);
}

bool fs::isWritable(const std::string &path)
{
    return !::access(path.c_str(), W_OK);
}

bool fs::isExecutable(const std::string &path)
{
    return !::access(path.c_str(), X_OK);
}

// create
bool fs::touch(const std::string &file, time_t mtime, time_t atime)
{
    // using current time if it's zero
    if (!mtime)
        mtime = ::time(nullptr);

    // using mtime if it's zero
    if (!atime)
        atime = mtime;

    // create directory
    fs::create(fs::dirname(file));

    // create file if not exist
    FILE *fp = ::fopen(file.c_str(), "ab+");
    if (!fp)
        return false;

    ::fclose(fp);

    // modify mtime and atime
    struct stat st = {0};

    if (!::stat(file.c_str(), &st))
    {
        struct utimbuf time = {0};

        time.modtime = mtime;
        time.actime  = atime;

        return !::utime(file.c_str(), &time);
    }
    else
    {
        return false;
    }
}

bool fs::create(const std::string &dir, std::uint16_t mode, bool recursive)
{
    if (!mode)
        mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

    if (!fs::isDir(dir))
    {
        if (recursive)
        {
            auto success = true;
            auto dirname = fs::dirname(dir);

            if (!dirname.empty())
                success = fs::create(dirname, mode, recursive) && success;

            return !::mkdir(dir.c_str(), mode) && success;
        }
        else
        {
            return !::mkdir(dir.c_str(), mode);
        }
    }
    else
    {
        return true;
    }
}

bool fs::remove(const std::string &path)
{
    if (fs::isFile(path))
    {
        return !::remove(path.c_str());
    }
    else
    {
        DIR    *dir = ::opendir(path.c_str());
        dirent *cur = nullptr;

        if (!dir)
            return false;

        auto ok  = true;
        auto sep = fs::separator();

        // remove sub items
        while ((cur = ::readdir(dir)))
        {
            std::string name(cur->d_name);

            if ((name == ".") || (name == ".."))
                continue;

            std::string full(*(path.end() - 1) == sep ? path + name : path + sep + name);

            if (fs::isDir(full))
                ok = fs::remove(full) && ok;
            else
                ok = !::remove(full.c_str()) && ok;
        }

        ::closedir(dir);

        // remove itself
        if (ok)
            ok = !::rmdir(path.c_str());

        return ok;
    }
}

// change
bool fs::change(const std::string &directory)
{
    return !::chdir(directory.c_str());
}

// visit
void fs::visit(const std::string &directory,
                 std::function<bool (const std::string &path)> callback,
                 bool recursive)
{
    DIR    *dir = ::opendir(directory.c_str());
    dirent *cur = nullptr;

    if (!dir)
        return;

    auto sep = fs::separator();

    while ((cur = ::readdir(dir)))
    {
        std::string name(cur->d_name);

        if ((name == ".") || (name == ".."))
            continue;

        std::string full(*(directory.end() - 1) == sep ? directory + name : directory + sep + name);

        if (!callback(full))
            break;

        if (recursive && fs::isDir(full))
            fs::visit(full, callback, recursive);
    }

    ::closedir(dir);
}