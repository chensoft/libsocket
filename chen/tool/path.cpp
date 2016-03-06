/**
 * Created by Jian Chen
 * @since  2016.01.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "path.hpp"
#include "log.hpp"
#include "str.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <utime.h>
#include <pwd.h>

using namespace chen;

// -----------------------------------------------------------------------------
// path
std::string path::root()
{
#if defined(CHEN_OS_WIN32)
#error todo
#else
    return "/";
#endif
}

std::string path::home()
{
    // check env variable first
    auto env = ::getenv("HOME");
    if (env)
        return env;

    // check login info
    struct passwd *pw = ::getpwuid(::getuid());
    return pw->pw_dir;
}

std::string path::temp()
{
#if defined(CHEN_OS_WIN32)
#error todo maybe GetTempPath API function
#else
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
#endif
}

std::string path::current()
{
    char cwd[PATH_MAX] = {0};
    ::getcwd(cwd, PATH_MAX);

    return std::string(cwd);
}

std::vector<std::string> path::drives()
{
#if defined(CHEN_OS_WIN32)
#error todo
#else
    return {"/"};
#endif
}

char path::separator()
{
    return '/';
}

std::string path::realpath(const std::string &path)
{
    char buf[PATH_MAX] = {0};
    ::realpath(path.c_str(), buf);

    std::string ret(buf);
    return path::isExist(ret) ? ret : "";
}

std::string path::absolute(const std::string &path)
{
    if (path::isRelative(path))
        return path::current() + path::separator() + path::normalize(path);
    else
        return path::normalize(path);
}

std::string path::normalize(const std::string &path)
{
    if (path.empty())
        return "";

    auto sep = path::separator();
    auto abs = (path[0] == sep);

    std::size_t ptr = 0;  // current segment cursor
    std::size_t len = 0;  // current segment length

    std::vector<std::pair<std::size_t, std::size_t>> store;  // segments cache

    for (std::size_t i = 0, l = path.size(); (i < l) || len; ++i)
    {
        if ((i == l) || (path[i] == sep))
        {
            if (len > 0)
            {
                // store a segment
                if (str::equal(&path[ptr], len, "..", 2))
                {
                    if (!store.empty() && !str::equal(&path[store.back().first], store.back().second, "..", 2))
                        store.pop_back();
                    else
                        store.push_back(std::make_pair(ptr, len));
                }
                else if (!str::equal(&path[ptr], len, ".", 1))
                {
                    store.push_back(std::make_pair(ptr, len));
                }

                ptr = len = 0;
            }
        }
        else
        {
            if (!len)
            {
                ptr = i;
                len = 1;
            }
            else
            {
                ++len;
            }
        }
    }

    // concat
    std::string ret(abs ? "/" : "");

    for (std::size_t i = 0, l = store.size(); i < l; ++i)
    {
        auto &pair = store[i];
        ret.append(&path[pair.first], pair.second);

        if (i < l - 1)
            ret.append(1, sep);
    }

    return ret;
}

std::string path::dirname(const std::string &path)
{
    if (path.empty())
        return "";

    auto sep = path::separator();
    auto beg = path.rbegin();
    auto end = path.rend();
    auto idx = beg;

    auto flag = false;

    for (; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            auto next = idx + 1;

            if (flag && (*next != sep))
            {
                ++idx;
                break;
            }
        }
        else if (!flag)
        {
            flag = true;
        }
    }

    if (flag)
    {
        if (idx != end)
        {
            return path.substr(0, static_cast<std::size_t>(end - idx));
        }
        else
        {
            auto first = path[0];
            return (first == sep) ? "/" : ".";  // using root directory or current directory
        }
    }
    else
    {
        return "/";  // all chars are '/'
    }
}

std::string path::basename(const std::string &path)
{
    auto sep = path::separator();
    auto beg = path.rbegin();
    auto end = path.rend();

    auto flag_a = end;  // before
    auto flag_b = end;  // after

    for (auto idx = beg; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            if (flag_b != end)
            {
                flag_a = idx;
                break;
            }
        }
        else if (flag_b == end)
        {
            flag_b = idx;
        }
    }

    if (flag_b != end)
        return path.substr(static_cast<std::size_t>(end - flag_a), static_cast<std::size_t>(flag_a - flag_b));
    else
        return "";
}

std::string path::extname(const std::string &path, std::size_t dots)
{
    if (!dots)
        return "";

    auto sep = path::separator();
    auto beg = path.rbegin();
    auto end = path.rend();
    auto idx = beg;
    auto cur = beg;
    auto num = 0;

    for (; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            break;
        }
        else if (*idx == '.')
        {
            cur = idx + 1;

            if (++num == dots)
                break;
        }
    }

    return num ? path.substr(static_cast<std::size_t>(end - cur), static_cast<std::size_t>(cur - beg)) : "";
}

// exist
bool path::isExist(const std::string &path)
{
    return !::access(path.c_str(), F_OK);
}

bool path::isDir(const std::string &path, bool strict)
{
    struct stat st = {0};
    auto ok = strict ? !::lstat(path.c_str(), &st) : !::stat(path.c_str(), &st);
    return ok && S_ISDIR(st.st_mode);
}

bool path::isFile(const std::string &path, bool strict)
{
    struct stat st = {0};
    auto ok = strict ? !::lstat(path.c_str(), &st) : !::stat(path.c_str(), &st);
    return ok && S_ISREG(st.st_mode);
}

bool path::isLink(const std::string &path)
{
    struct stat st = {0};
    return !::lstat(path.c_str(), &st) && S_ISLNK(st.st_mode);
}

bool path::isAbsolute(const std::string &path)
{
    auto sep = path::separator();
    return !path.empty() && (path[0] == sep);
}

bool path::isRelative(const std::string &path)
{
    return !path::isAbsolute(path);
}

bool path::isReadable(const std::string &path)
{
    return !::access(path.c_str(), R_OK);
}

bool path::isWritable(const std::string &path)
{
    return !::access(path.c_str(), W_OK);
}

bool path::isExecutable(const std::string &path)
{
    return !::access(path.c_str(), X_OK);
}

// time
time_t path::atime(const std::string &path)
{
    struct stat st = {0};
    return !::stat(path.c_str(), &st) ? st.st_atime : 0;
}

time_t path::mtime(const std::string &path)
{
    struct stat st = {0};
    return !::stat(path.c_str(), &st) ? st.st_mtime : 0;
}

time_t path::ctime(const std::string &path)
{
    struct stat st = {0};
    return !::stat(path.c_str(), &st) ? st.st_ctime : 0;
}

// size
off_t path::filesize(const std::string &file)
{
    struct stat st = {0};
    return !::stat(file.c_str(), &st) ? st.st_size : 0;
}

// create
bool path::touch(const std::string &file, time_t mtime, time_t atime)
{
    // using current time if it's zero
    if (!mtime)
        mtime = ::time(nullptr);

    // using mtime if it's zero
    if (!atime)
        atime = mtime;

    // create directory
    path::create(path::dirname(file));

    // create file if not exist
    FILE *fp = ::fopen(file.c_str(), "ab+");
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

bool path::create(const std::string &dir, mode_t mode)
{
    if (!mode)
        mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

    if (!path::isDir(dir))
    {
        auto success = true;
        auto dirname = path::dirname(dir);

        if (!dirname.empty())
            success = path::create(dirname, mode) && success;

        return !::mkdir(dir.c_str(), mode) && success;
    }
    else
    {
        return true;
    }
}

bool path::rename(const std::string &path_old, const std::string &path_new)
{
    // remove new path if it's already exist
    if (!path::remove(path_new))
        path::create(path::dirname(path_new));  // create new directory

    // rename old path to new path
    return !::rename(path_old.c_str(), path_new.c_str());
}

bool path::remove(const std::string &path)
{
    if (path::isFile(path))
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
        auto sep = path::separator();

        // remove sub items
        while ((cur = ::readdir(dir)))
        {
            std::string name(cur->d_name);

            if ((name == ".") || (name == ".."))
                continue;

            std::string full(*path.end() == sep ? path + name : path + sep + name);

            if (path::isDir(full))
                ok = path::remove(full) && ok;
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
bool path::change(const std::string &directory)
{
    return !::chdir(directory.c_str());
}

// visit
void path::visit(const std::string &directory,
                 bool recursive,
                 std::function<void (const std::string &path)> callback)
{
    DIR    *dir = ::opendir(directory.c_str());
    dirent *cur = nullptr;

    if (!dir)
        return;

    auto sep = path::separator();

    while ((cur = ::readdir(dir)))
    {
        std::string name(cur->d_name);

        if ((name == ".") || (name == ".."))
            continue;

        std::string full(*directory.end() == sep ? directory + name : directory + sep + name);

        callback(full);

        if (recursive && path::isDir(full))
            path::visit(full, recursive, callback);
    }

    ::closedir(dir);
}

std::vector<std::string> path::visit(const std::string &directory, bool recursive)
{
    std::vector<std::string> store;

    path::visit(directory, recursive, [&store] (const std::string &path) {
        store.push_back(path);
    });

    return store;
}

// count
std::size_t path::count(const std::string &directory,
                        bool recursive,
                        bool contain_file,
                        bool contain_dir)
{
    if (!contain_file && !contain_dir)
        return 0;

    std::size_t num = 0;

    path::visit(directory, recursive, [&] (const std::string &path) {
        if ((contain_file && contain_dir) ||
            (contain_file && path::isFile(path)) ||
            (contain_dir && path::isDir(path)))
        {
            ++num;
        }
    });

    return num;
}