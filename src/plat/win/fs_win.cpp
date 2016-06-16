/**
 * Created by Jian Chen
 * @since  2016.06.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/fs.hpp>
#include <sys/stat.h>
#include <Windows.h>
#include <Userenv.h>

#pragma comment(lib, "userenv.lib")

using namespace chen;

// -----------------------------------------------------------------------------
// fs
std::string fs::root()
{
	CHAR buf[MAX_PATH] = { 0 };

	if (::GetSystemWindowsDirectory(buf, sizeof(buf)) >= 3)
		return std::string(buf, 3);
	else
		return "";
}

std::string fs::home()
{
    CHAR buf[MAX_PATH] = { 0 };

    HANDLE token = 0;
    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token))
        return "";

    DWORD size = sizeof(buf);
    BOOL    ok = ::GetUserProfileDirectory(token, buf, &size);

    CloseHandle(token);

    return ok ? buf : "";
}

std::string fs::temp()
{
    CHAR buf[MAX_PATH] = { 0 };

    if (::GetTempPath(sizeof(buf), buf))
        return buf;
    else
        return "";
}

std::string fs::current()
{
    CHAR buf[MAX_PATH] = { 0 };

    if (::GetCurrentDirectory(sizeof(buf), buf))
        return buf;
    else
        return "";
}

std::vector<std::string> fs::drives()
{
    std::vector<std::string> ret;

    DWORD bits = ::GetLogicalDrives();

    for (auto i = 0; i < 26; ++i)  // a to z
    {
        if (bits & (1 << i))
            ret.push_back(std::string(1, ('A' + i)) + ":\\");
    }

    return ret;
}

char fs::separator()
{
    return '\\';
}

std::string fs::realpath(const std::string &path)
{
    return "";
}

// exist
bool fs::isExist(const std::string &path)
{
    return false;
}

bool fs::isDir(const std::string &path, bool strict)
{
    return false;
}

bool fs::isFile(const std::string &path, bool strict)
{
    return false;
}

bool fs::isLink(const std::string &path)
{
    return false;
}

bool fs::isReadable(const std::string &path)
{
    return false;
}

bool fs::isWritable(const std::string &path)
{
    return false;
}

bool fs::isExecutable(const std::string &path)
{
    return false;
}

// time
time_t fs::atime(const std::string &path)
{
    return 0;
}

time_t fs::mtime(const std::string &path)
{
    return 0;
}

time_t fs::ctime(const std::string &path)
{
    return 0;
}

// size
off_t fs::filesize(const std::string &file)
{
	struct stat st = { 0 };
	return !::stat(file.c_str(), &st) ? st.st_size : 0;
}

// create
bool fs::touch(const std::string &file, time_t mtime, time_t atime)
{
    return false;
}

bool fs::create(const std::string &dir, std::uint16_t mode, bool recursive)
{
    return false;
}

bool fs::rename(const std::string &path_old, const std::string &path_new)
{
    return false;
}

bool fs::remove(const std::string &path)
{
    return false;
}

// change
bool fs::change(const std::string &directory)
{
    return false;
}

// visit
void fs::visit(const std::string &directory,
               std::function<bool (const std::string &path)> callback,
               bool recursive)
{
}