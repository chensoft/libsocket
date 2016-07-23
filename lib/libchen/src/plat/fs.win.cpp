/**
 * Created by Jian Chen
 * @since  2016.06.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <chen/sys/fs.hpp>
#include <sys/utime.h>
#include <sys/stat.h>
#include <Windows.h>
#include <Userenv.h>
#include <time.h>
#include <io.h>

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

    ::CloseHandle(token);

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
            ret.emplace_back(std::string(1, ('A' + i)) + ":\\");
    }

    return ret;
}

char fs::separator()
{
    return '\\';
}

std::string fs::realpath(const std::string &path)
{
    CHAR buf[MAX_PATH] = { 0 };
    DWORD size = sizeof(buf);

    if (::GetFullPathName(fs::absolute(path).c_str(), size, buf, NULL))
    {
        std::string ret(buf);
        return fs::isExist(ret) ? ret : "";
    }
    else
    {
        return "";
    }
}

// exist
bool fs::isExist(const std::string &path)
{
    DWORD attr = ::GetFileAttributes(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES;
}

bool fs::isDir(const std::string &path, bool strict)
{
    DWORD attr = ::GetFileAttributes(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool fs::isFile(const std::string &path, bool strict)
{
    DWORD attr = ::GetFileAttributes(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool fs::isLink(const std::string &path)
{
    return false;
}

bool fs::isReadable(const std::string &path)
{
    // see https://msdn.microsoft.com/en-us/library/1w06ktdy.aspx
    // 0: Existence only, 2: Write-only, 4: Read-only, 6: Read and write
    return !::_access(path.c_str(), 6) || !::_access(path.c_str(), 4);
}

bool fs::isWritable(const std::string &path)
{
    return !::_access(path.c_str(), 6) || !::_access(path.c_str(), 2);
}

bool fs::isExecutable(const std::string &path)
{
    DWORD type = 0;
    return ::GetBinaryType(path.c_str(), &type) == TRUE;
}

// create
bool fs::touch(const std::string &file, std::time_t mtime, std::time_t atime)
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
    FILE *fp = NULL;
    ::fopen_s(&fp, file.c_str(), "ab+");

    if (!fp)
        return false;

    ::fclose(fp);

    // modify mtime and atime
    struct stat st = { 0 };

    if (!::stat(file.c_str(), &st))
    {
        struct _utimbuf time = { 0 };

        time.modtime = mtime;
        time.actime = atime;

        return !::_utime(file.c_str(), &time);
    }
    else
    {
        return false;
    }
}

bool fs::create(const std::string &dir, std::uint16_t mode, bool recursive)
{
    // ignore mode
    if (!fs::isDir(dir))
    {
        if (recursive)
        {
            auto success = true;
            auto dirname = fs::dirname(dir);

            if (!dirname.empty())
                success = fs::create(dirname, mode, recursive) && success;

            return (::CreateDirectory(dir.c_str(), NULL) == TRUE) && success;
        }
        else
        {
            return ::CreateDirectory(dir.c_str(), NULL) == TRUE;
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
        return ::DeleteFile(path.c_str()) == TRUE;
    }
    else
    {
        WIN32_FIND_DATA data;
        HANDLE find = ::FindFirstFile((path + "\\*").c_str(), &data);

        if (find == INVALID_HANDLE_VALUE)
            return false;

        auto ok = true;
        auto sep = fs::separator();

        // remove sub items
        do
        {
            std::string name(data.cFileName);

            if ((name == ".") || (name == ".."))
                continue;

            std::string full(*(path.end() - 1) == sep ? path + name : path + sep + name);

            if (fs::isDir(full))
                ok = fs::remove(full) && ok;
            else
                ok = (::DeleteFile(full.c_str()) == TRUE) && ok;
        } while (::FindNextFile(find, &data));

        ::FindClose(find);

        // remove itself
        if (ok)
            ok = ::RemoveDirectory(path.c_str()) == TRUE;

        return ok;
    }
}

// change
bool fs::change(const std::string &directory)
{
    return ::SetCurrentDirectory(directory.c_str()) == TRUE;
}

// visit
void fs::visit(const std::string &directory,
               std::function<bool (const std::string &path)> callback,
               bool recursive)
{
    WIN32_FIND_DATA data;
    HANDLE find = ::FindFirstFile((directory + "\\*").c_str(), &data);

    if (find == INVALID_HANDLE_VALUE)
        return;

    auto sep = fs::separator();

    do
    {
        std::string name(data.cFileName);

        if ((name == ".") || (name == ".."))
            continue;

        std::string full(*(directory.end() - 1) == sep ? directory + name : directory + sep + name);

        if (!callback(full))
            break;

        if (recursive && fs::isDir(full))
            fs::visit(full, callback, recursive);
    } while (::FindNextFile(find, &data));

    ::FindClose(find);
}

#endif